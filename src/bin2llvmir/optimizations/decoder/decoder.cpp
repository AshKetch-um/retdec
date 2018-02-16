/**
* @file src/bin2llvmir/optimizations/decoder/decoder.cpp
* @brief Decode input binary into LLVM IR.
* @copyright (c) 2017 Avast Software, licensed under the MIT license
*/

#include <iostream>
#include <map>

#include <llvm/IR/InstIterator.h>

#include "retdec/utils/conversion.h"
#include "retdec/utils/string.h"
#include "retdec/bin2llvmir/optimizations/decoder/decoder.h"
#include "retdec/bin2llvmir/utils/defs.h"
#define debug_enabled true
#include "retdec/llvm-support/utils.h"

using namespace retdec::llvm_support;
using namespace retdec::utils;
using namespace retdec::capstone2llvmir;
using namespace llvm;
using namespace retdec::fileformat;

namespace retdec {
namespace bin2llvmir {

char Decoder::ID = 0;

static RegisterPass<Decoder> X(
		"decoder",
		"Input binary to LLVM IR decoding",
		false, // Only looks at CFG
		false // Analysis Pass
);

Decoder::Decoder() :
		ModulePass(ID)
{

}

bool Decoder::runOnModule(Module& m)
{
	_module = &m;
	_config = ConfigProvider::getConfig(_module);
	_image = FileImageProvider::getFileImage(_module);
	_debug = DebugFormatProvider::getDebugFormat(_module);
	return runCatcher();
}

bool Decoder::runOnModuleCustom(
		llvm::Module& m,
		Config* c,
		FileImage* o,
		DebugFormat* d)
{
	_module = &m;
	_config = c;
	_image = o;
	_debug = d;
	return runCatcher();
}

bool Decoder::runCatcher()
{
	try
	{
		return run();
	}
	catch (const Capstone2LlvmIrBaseError& e)
	{
		LOG << "[capstone2llvmir]: " << e.what() << std::endl;
		return false;
	}
}

bool Decoder::run()
{
	if (_config == nullptr || _image == nullptr)
	{
		LOG << "[ABORT] Config or object image is not available.\n";
		return false;
	}
	if (initTranslator())
	{
		LOG << "[ABORT] Unable to create capstone2llvmir translator.\n";
		return false;
	}

	initEnvironment();
	initRanges();
	initJumpTargets();

	LOG << std::endl;
	LOG << "Allowed ranges:" << std::endl;
	LOG << _allowedRanges << std::endl;
	LOG << std::endl;
	LOG << "Alternative ranges:" << std::endl;
	LOG << _alternativeRanges << std::endl;
	LOG << "Jump targets:" << std::endl;
	LOG << _jumpTargets << std::endl;
	LOG << std::endl;

	doDecoding();

	// TODO:
	dumpModuleToFile(_module);
	exit(1);
	return false;
}

/**
 * Initialize capstone2llvmir translator according to the architecture of
 * file to decompile.
 * @return @c True if error, @c false otherwise.
 */
bool Decoder::initTranslator()
{
	auto& a = _config->getConfig().architecture;

	cs_arch arch = CS_ARCH_ALL;
	cs_mode basicMode = CS_MODE_LITTLE_ENDIAN;
	cs_mode extraMode = a.isEndianBig()
			? CS_MODE_BIG_ENDIAN
			: CS_MODE_LITTLE_ENDIAN;

	if (a.isX86())
	{
		arch = CS_ARCH_X86;
		switch (_config->getConfig().architecture.getBitSize())
		{
			case 16: basicMode = CS_MODE_16; break;
			case 64: basicMode = CS_MODE_64; break;
			default:
			case 32: basicMode = CS_MODE_32; break;
		}
	}
	else if (_config->getConfig().architecture.isMipsOrPic32())
	{
		arch = CS_ARCH_MIPS;
		switch (_config->getConfig().architecture.getBitSize())
		{
			case 64: basicMode = CS_MODE_MIPS64; break;
			default:
			case 32: basicMode = CS_MODE_MIPS32; break;
		}
	}
	else if (_config->getConfig().architecture.isPpc())
	{
		arch = CS_ARCH_PPC;
		switch (_config->getConfig().architecture.getBitSize())
		{
			case 64: basicMode = CS_MODE_64; break;
			default:
			case 32: basicMode = CS_MODE_32; break;
		}
	}
	else if (_config->getConfig().architecture.isArmOrThumb()
			&& _config->getConfig().architecture.getBitSize() == 32)
	{
		arch = CS_ARCH_ARM;
		basicMode = CS_MODE_ARM;
	}

	_c2l = Capstone2LlvmIrTranslator::createArch(
			arch,
			_module,
			basicMode,
			extraMode);
	return false;
}

/**
 * Synchronize metadata between capstone2llvmir and bin2llvmir.
 */
void Decoder::initEnvironment()
{
	initEnvironmentAsm2LlvmMapping();
	initEnvironmentPseudoFunctions();
	initEnvironmentRegisters();
}

/**
 * Find out from capstone2llvmir which global is used for
 * LLVM IR <-> Capstone ASM mapping.
 * 1. Set its name.
 * 2. Set it to config.
 * 3. Create metadata for it, so it can be quickly recognized without querying
 *    config.
 */
void Decoder::initEnvironmentAsm2LlvmMapping()
{
	auto* a2lGv = _c2l->getAsm2LlvmMapGlobalVariable();
	a2lGv->setName(_asm2llvmGv);

	_config->setLlvmToAsmGlobalVariable(a2lGv);

	auto* nmd = _module->getOrInsertNamedMetadata(_asm2llvmMd);
	auto* mdString = MDString::get(_module->getContext(), a2lGv->getName());
	auto* mdn = MDNode::get(_module->getContext(), {mdString});
	nmd->addOperand(mdn);
}

/**
 * Set pseudo functions' names in LLVM IR and set them to config.
 */
void Decoder::initEnvironmentPseudoFunctions()
{
	auto* cf = _c2l->getCallFunction();
	cf->setName(_callFunction);
	_config->setLlvmCallPseudoFunction(cf);

	auto* rf = _c2l->getReturnFunction();
	rf->setName(_returnFunction);
	_config->setLlvmReturnPseudoFunction(rf);

	auto* bf = _c2l->getBranchFunction();
	bf->setName(_branchFunction);
	_config->setLlvmBranchPseudoFunction(bf);

	auto* cbf = _c2l->getCondBranchFunction();
	cbf->setName(_condBranchFunction);
	_config->setLlvmCondBranchPseudoFunction(cbf);

	if (auto* c2lX86 = dynamic_cast<Capstone2LlvmIrTranslatorX86*>(_c2l.get()))
	{
		c2lX86->getX87DataLoadFunction()->setName(_x87dataLoadFunction);
		c2lX86->getX87TagLoadFunction()->setName(_x87tagLoadFunction);
		c2lX86->getX87DataStoreFunction()->setName(_x87dataStoreFunction);
		c2lX86->getX87TagStoreFunction()->setName(_x87tagStoreFunction);
	}
}

/**
 * Create config objects for HW registers.
 */
void Decoder::initEnvironmentRegisters()
{
	for (GlobalVariable& gv : _module->getGlobalList())
	{
		if (_c2l->isRegister(&gv))
		{
			unsigned regNum = _c2l->getCapstoneRegister(&gv);
			auto s = retdec::config::Storage::inRegister(
					gv.getName(),
					regNum,
					"");

			retdec::config::Object cr(gv.getName(), s);
			cr.type.setLlvmIr(llvmObjToString(gv.getValueType()));
			cr.setRealName(gv.getName());
			_config->getConfig().registers.insert(cr);
		}
	}
}

/**
 *
 */
void Decoder::initRanges()
{
	LOG << "\n initRanges():" << std::endl;

	if (!_config->getConfig().parameters.isSelectedDecodeOnly())
	{
		initAllowedRangesWithSegments();
	}
}

/**
 *
 */
void Decoder::initAllowedRangesWithSegments()
{
	LOG << "\n initAllowedRangesWithSegments():" << std::endl;

	auto* epSeg = _image->getImage()->getEpSegment();
	for (auto& seg : _image->getSegments())
	{
		auto* sec = seg->getSecSeg();
		Address start = seg->getAddress();
		Address end = seg->getPhysicalEndAddress();

		LOG << "\t" << seg->getName() << " @ " << start << " -- "
				<< end << std::endl;

		if (start == end)
		{
			LOG << "\t\tsize == 0 -> skipped" << std::endl;
			continue;
		}

		if (seg.get() != epSeg && sec)
		{
			if (auto* s = dynamic_cast<const PeCoffSection*>(sec))
			{
				if (s->getPeCoffFlags() & PeLib::PELIB_IMAGE_SCN_MEM_DISCARDABLE)
				{
					LOG << "\t\t" << "PeLib::PELIB_IMAGE_SCN_MEM_DISCARDABLE"
							" -> skipped" << std::endl;
					continue;
				}
			}
		}

		if (sec)
		{
			switch (sec->getType())
			{
				case SecSeg::Type::CODE:
					LOG << "\t\tcode section -> allowed ranges"
							<< std::endl;
					_allowedRanges.insert(start, end);
					break;
				case SecSeg::Type::DATA:
					LOG << "\t\tdata section -> alternative ranges"
							<< std::endl;
					_alternativeRanges.insert(start, end);
					break;
				case SecSeg::Type::CODE_DATA:
					LOG << "\t\tcode/data section -> alternative ranges"
							<< std::endl;
					_alternativeRanges.insert(start, end);
					break;
				case SecSeg::Type::CONST_DATA:
					if (seg.get() == epSeg)
					{
						LOG << "\t\tconst data section == ep seg "
								"-> alternative ranges" << std::endl;
						_alternativeRanges.insert(start, end);
					}
					else
					{
						LOG << "\t\tconst data section -> alternative ranges"
								<< std::endl;
						continue;
					}
					break;
				case SecSeg::Type::UNDEFINED_SEC_SEG:
					LOG << "\t\tundef section -> alternative ranges"
							<< std::endl;
					_alternativeRanges.insert(start, end);
					break;
				case SecSeg::Type::BSS:
					LOG << "\t\tbss section -> skipped" << std::endl;
					continue;
				case SecSeg::Type::DEBUG:
					LOG << "\t\tdebug section -> skipped" << std::endl;
					continue;
				case SecSeg::Type::INFO:
					LOG << "\t\tinfo section -> skipped" << std::endl;
					continue;
				default:
					assert(false && "unhandled section type");
					continue;
			}
		}
		else if (seg.get() == epSeg)
		{
			LOG << "\t\tno underlying section or segment && ep seg "
					"-> alternative ranges" << std::endl;
			_alternativeRanges.insert(start, end);
		}
		else
		{
			LOG << "\t\tno underlying section or segment -> skipped"
					<< std::endl;
			continue;
		}
	}

	for (auto& seg : _image->getSegments())
	{
		auto& rc = seg->getNonDecodableAddressRanges();
		for (auto& r : rc)
		{
			if (!r.contains(_config->getConfig().getEntryPoint()))
			{
				_allowedRanges.remove(r.getStart(), r.getEnd());
				_alternativeRanges.remove(r.getStart(), r.getEnd());
			}
		}
	}
}

void Decoder::initJumpTargets()
{
	LOG << "\n initJumpTargets():" << std::endl;

	auto mode = _c2l->getBasicMode();

	// Entry point.
	//
	LOG << "\tEntry point:" << std::endl;
	auto ep = _config->getConfig().getEntryPoint();
	if (ep.isDefined())
	{
		LOG << "\t\t ep @ " << ep << std::endl;

		if (isArmOrThumb())
		{
			mode = ep % 2 ? CS_MODE_THUMB : CS_MODE_ARM;
		}

		_jumpTargets.push(
				ep,
				JumpTarget::eType::ENTRY_POINT,
				mode,
				_entryPointFunction);
	}
}

bool Decoder::isArmOrThumb() const
{
	return _config->getConfig().architecture.isArmOrThumb();
}

void Decoder::doDecoding()
{
	LOG << "\n doDecoding()" << std::endl;

	while (!_jumpTargets.empty())
	{
		JumpTarget jt = _jumpTargets.top();
		_jumpTargets.pop();
		LOG << "\tprocessing : " << jt << std::endl;

		Address start = jt.address;
		auto* range = _allowedRanges.getRange(start);
		if (range == nullptr)
		{
			LOG << "\t\tfound no range -> skipped " << std::endl;
			continue;
		}
	}
}

} // namespace bin2llvmir
} // namespace retdec
