/**
 * @file src/capstone2llvmir/capstone2llvmir.cpp
 * @brief Converts bytes to Capstone representation, and Capstone representation
 *        to LLVM IR.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#include <iomanip>
#include <iostream>

#include "capstone2llvmir/arm/arm_impl.h"
#include "capstone2llvmir/mips/mips_impl.h"
#include "capstone2llvmir/powerpc/powerpc_impl.h"
#include "capstone2llvmir/x86/x86_impl.h"
#include "capstone2llvmir/capstone2llvmir_impl.h"

namespace retdec {
namespace capstone2llvmir {

//
//==============================================================================
// Capstone2LlvmIrTranslator
//==============================================================================
//

std::unique_ptr<Capstone2LlvmIrTranslator> Capstone2LlvmIrTranslator::createArch(
		cs_arch a,
		llvm::Module* m,
		cs_mode basic,
		cs_mode extra)
{
	switch (a)
	{
		case CS_ARCH_ARM:
		{
			if (basic == CS_MODE_ARM) return createArm(m, extra);
			if (basic == CS_MODE_THUMB) return createThumb(m, extra);
			break;
		}
		case CS_ARCH_ARM64:
		{
			return createArm64(m, extra);
		}
		case CS_ARCH_MIPS:
		{
			if (basic == CS_MODE_MIPS32) return createMips32(m, extra); // == CS_MODE_32
			if (basic == CS_MODE_MIPS64) return createMips64(m, extra); // == CS_MODE_64
			if (basic == CS_MODE_MIPS3) return createMips3(m, extra);
			if (basic == CS_MODE_MIPS32R6) return createMips32R6(m, extra);
			break;
		}
		case CS_ARCH_X86:
		{
			if (basic == CS_MODE_16) return createX86_16(m, extra);
			if (basic == CS_MODE_32) return createX86_32(m, extra);
			if (basic == CS_MODE_64) return createX86_64(m, extra);
			break;
		}
		case CS_ARCH_PPC:
		{
			if (basic == CS_MODE_32) return createPpc32(m, extra);
			if (basic == CS_MODE_64) return createPpc64(m, extra);
			break;
		}
		case CS_ARCH_SPARC:
		{
			return createSparc(m, extra);
		}
		case CS_ARCH_SYSZ:
		{
			return createSysz(m, extra);
		}
		case CS_ARCH_XCORE:
		{
			return createXcore(m, extra);
		}
		default:
		{
			// Nothing.
			break;
		}
	}

	throw Capstone2LlvmIrError("Unhandled Capstone architecture or mode.");
}

std::unique_ptr<Capstone2LlvmIrTranslator> Capstone2LlvmIrTranslator::createArm(
		llvm::Module* m,
		cs_mode extra)
{
	return std::make_unique<Capstone2LlvmIrTranslatorArm_impl>(m, CS_MODE_ARM, extra);
}

std::unique_ptr<Capstone2LlvmIrTranslator> Capstone2LlvmIrTranslator::createThumb(
		llvm::Module* m,
		cs_mode extra)
{
	return std::make_unique<Capstone2LlvmIrTranslatorArm_impl>(m, CS_MODE_THUMB, extra);
}

std::unique_ptr<Capstone2LlvmIrTranslator> Capstone2LlvmIrTranslator::createArm64(
		llvm::Module* m,
		cs_mode extra)
{
	assert(false && "not implemented");
	return nullptr;
}

std::unique_ptr<Capstone2LlvmIrTranslator> Capstone2LlvmIrTranslator::createMips32(
		llvm::Module* m,
		cs_mode extra)
{
	return std::make_unique<Capstone2LlvmIrTranslatorMips_impl>(m, CS_MODE_MIPS32, extra);
}

std::unique_ptr<Capstone2LlvmIrTranslator> Capstone2LlvmIrTranslator::createMips64(
		llvm::Module* m,
		cs_mode extra)
{
	return std::make_unique<Capstone2LlvmIrTranslatorMips_impl>(m, CS_MODE_MIPS64, extra);
}

std::unique_ptr<Capstone2LlvmIrTranslator> Capstone2LlvmIrTranslator::createMips3(
		llvm::Module* m,
		cs_mode extra)
{
	return std::make_unique<Capstone2LlvmIrTranslatorMips_impl>(m, CS_MODE_MIPS3, extra);
}

std::unique_ptr<Capstone2LlvmIrTranslator> Capstone2LlvmIrTranslator::createMips32R6(
		llvm::Module* m,
		cs_mode extra)
{
	return std::make_unique<Capstone2LlvmIrTranslatorMips_impl>(m, CS_MODE_MIPS32R6, extra);
}

std::unique_ptr<Capstone2LlvmIrTranslator> Capstone2LlvmIrTranslator::createX86_16(
		llvm::Module* m,
		cs_mode extra)
{
	return std::make_unique<Capstone2LlvmIrTranslatorX86_impl>(m, CS_MODE_16, extra);
}

std::unique_ptr<Capstone2LlvmIrTranslator> Capstone2LlvmIrTranslator::createX86_32(
		llvm::Module* m,
		cs_mode extra)
{
	return std::make_unique<Capstone2LlvmIrTranslatorX86_impl>(m, CS_MODE_32, extra);
}

std::unique_ptr<Capstone2LlvmIrTranslator> Capstone2LlvmIrTranslator::createX86_64(
		llvm::Module* m,
		cs_mode extra)
{
	return std::make_unique<Capstone2LlvmIrTranslatorX86_impl>(m, CS_MODE_64, extra);
}

std::unique_ptr<Capstone2LlvmIrTranslator> Capstone2LlvmIrTranslator::createPpc32(
		llvm::Module* m,
		cs_mode extra)
{
	return std::make_unique<Capstone2LlvmIrTranslatorPowerpc_impl>(m, CS_MODE_32, extra);
}

std::unique_ptr<Capstone2LlvmIrTranslator> Capstone2LlvmIrTranslator::createPpc64(
		llvm::Module* m,
		cs_mode extra)
{
	return std::make_unique<Capstone2LlvmIrTranslatorPowerpc_impl>(m, CS_MODE_64, extra);
}

std::unique_ptr<Capstone2LlvmIrTranslator> Capstone2LlvmIrTranslator::createPpcQpx(
		llvm::Module* m,
		cs_mode extra)
{
	return std::make_unique<Capstone2LlvmIrTranslatorPowerpc_impl>(m, CS_MODE_QPX, extra);
}

std::unique_ptr<Capstone2LlvmIrTranslator> Capstone2LlvmIrTranslator::createSparc(
		llvm::Module* m,
		cs_mode extra)
{
	assert(false && "not implemented");
	return nullptr;
}

std::unique_ptr<Capstone2LlvmIrTranslator> Capstone2LlvmIrTranslator::createSysz(
		llvm::Module* m,
		cs_mode extra)
{
	assert(false && "not implemented");
	return nullptr;
}

std::unique_ptr<Capstone2LlvmIrTranslator> Capstone2LlvmIrTranslator::createXcore(
		llvm::Module* m,
		cs_mode extra)
{
	assert(false && "not implemented");
	return nullptr;
}

Capstone2LlvmIrTranslator::~Capstone2LlvmIrTranslator()
{

}

//
//==============================================================================
// Capstone2LlvmIrTranslator_impl
//==============================================================================
//

Capstone2LlvmIrTranslator_impl::Capstone2LlvmIrTranslator_impl(
		cs_arch a,
		cs_mode basic,
		cs_mode extra,
		llvm::Module* m)
		:
		_arch(a),
		_basicMode(basic),
		_extraMode(extra),
		_module(m)
{
	// Do not call anything here, especially virtual methods.
}

Capstone2LlvmIrTranslator_impl::~Capstone2LlvmIrTranslator_impl()
{
	closeHandle();
}

void Capstone2LlvmIrTranslator_impl::initialize()
{
	if (!isAllowedBasicMode(_basicMode))
	{
		throw Capstone2LlvmIrModeError(
				_arch,
				_basicMode,
				Capstone2LlvmIrModeError::eType::BASIC_MODE);
	}
	if (!isAllowedExtraMode(_extraMode))
	{
		throw Capstone2LlvmIrModeError(
				_arch,
				_extraMode,
				Capstone2LlvmIrModeError::eType::EXTRA_MODE);
	}

	openHandle(); // Sets both _basicMode and _extraMode.
	configureHandle();

	initializeRegNameMap();
	initializeRegTypeMap();
	initializeArchSpecific();

	generateEnvironment();
}

void Capstone2LlvmIrTranslator_impl::openHandle()
{
	cs_mode finalMode = static_cast<cs_mode>(_basicMode + _extraMode);
	if (cs_open(_arch, finalMode, &_handle) != CS_ERR_OK)
	{
		throw CapstoneError(cs_errno(_handle));
	}
}

void Capstone2LlvmIrTranslator_impl::configureHandle()
{
	if (cs_option(_handle, CS_OPT_DETAIL, CS_OPT_ON) != CS_ERR_OK)
	{
		throw CapstoneError(cs_errno(_handle));
	}
}

void Capstone2LlvmIrTranslator_impl::closeHandle()
{
	if (_handle != 0)
	{
		if (cs_close(&_handle) != CS_ERR_OK)
		{
			throw CapstoneError(cs_errno(_handle));
		}
	}
}

void Capstone2LlvmIrTranslator_impl::generateEnvironment()
{
	generateSpecialAsm2LlvmMapGlobal();
	generateCallFunction();
	generateReturnFunction();
	generateBranchFunction();
	generateCondBranchFunction();

	generateEnvironmentArchSpecific();
	generateRegisters();
	generateDataLayout();
}

/**
 * The generated global variable is unnamed. capstone2llvmir library does not
 * allow to specify or set its name. Users can however get the variable with
 * @c getAsm2LlvmMapGlobalVariable() and do whatever they want with it
 * (e.g. rename).
 */
void Capstone2LlvmIrTranslator_impl::generateSpecialAsm2LlvmMapGlobal()
{
	llvm::GlobalValue::LinkageTypes lt = llvm::GlobalValue::InternalLinkage;
	llvm::Constant* initializer = nullptr;
	auto* t = llvm::IntegerType::getInt64Ty(_module->getContext());
	if (initializer == nullptr
			&& lt != llvm::GlobalValue::LinkageTypes::ExternalLinkage)
	{
		initializer = llvm::ConstantInt::get(t, 0);
	}

	_asm2llvmGv = new llvm::GlobalVariable(
			*_module,
			t,
			false, // isConstant
			lt,
			initializer);
}

bool Capstone2LlvmIrTranslator_impl::isSpecialAsm2LlvmMapGlobal(llvm::Value* v) const
{
	return _asm2llvmGv == v;
}

llvm::StoreInst* Capstone2LlvmIrTranslator_impl::generateSpecialAsm2LlvmInstr(
		llvm::IRBuilder<>& irb,
		cs_insn* i)
{
	retdec::utils::Address a = i->address;
	auto* gv = getAsm2LlvmMapGlobalVariable();
	auto* ci = llvm::ConstantInt::get(gv->getValueType(), a, false);
	auto* s = irb.CreateStore(ci, gv, true);

	auto* cip = llvm::ConstantInt::get(irb.getInt64Ty(), reinterpret_cast<uintptr_t>(i));
	auto* mdc = llvm::ConstantAsMetadata::get(cip);
	auto* mdn = llvm::MDNode::get(_module->getContext(), {mdc});
	s->setMetadata("asm", mdn);
	return s;
}

llvm::StoreInst* Capstone2LlvmIrTranslator_impl::isSpecialAsm2LlvmInstr(llvm::Value* v) const
{
	if (auto* s = llvm::dyn_cast<llvm::StoreInst>(v))
	{
		if (isSpecialAsm2LlvmMapGlobal(s->getPointerOperand()))
		{
			return s;
		}
	}
	return nullptr;
}

void Capstone2LlvmIrTranslator_impl::generateCallFunction()
{
	auto* ft = llvm::FunctionType::get(
			llvm::Type::getVoidTy(_module->getContext()),
			{llvm::Type::getIntNTy(_module->getContext(), getArchBitSize())},
			false);
	_callFunction = llvm::Function::Create(
			ft,
			llvm::GlobalValue::LinkageTypes::ExternalLinkage,
			"",
			_module);
}

llvm::CallInst* Capstone2LlvmIrTranslator_impl::generateCallFunctionCall(
		llvm::IRBuilder<>& irb,
		llvm::Value* t)
{
	auto* a1t = _callFunction->getArgumentList().front().getType();
	t = irb.CreateSExtOrTrunc(t, a1t);
	_branchGenerated = irb.CreateCall(_callFunction, {t});
	return _branchGenerated;
}

bool Capstone2LlvmIrTranslator_impl::isCallFunction(llvm::Function* f) const
{
	return f == _callFunction;
}

bool Capstone2LlvmIrTranslator_impl::isCallFunctionCall(llvm::CallInst* c) const
{
	return c ? isCallFunction(c->getCalledFunction()) : false;
}

void Capstone2LlvmIrTranslator_impl::generateReturnFunction()
{
	auto* ft = llvm::FunctionType::get(
			llvm::Type::getVoidTy(_module->getContext()),
			{llvm::Type::getIntNTy(_module->getContext(), getArchBitSize())},
			false);
	_returnFunction = llvm::Function::Create(
			ft,
			llvm::GlobalValue::LinkageTypes::ExternalLinkage,
			"",
			_module);
}

llvm::CallInst* Capstone2LlvmIrTranslator_impl::generateReturnFunctionCall(
		llvm::IRBuilder<>& irb,
		llvm::Value* t)
{
	auto* a1t = _returnFunction->getArgumentList().front().getType();
	t = irb.CreateSExtOrTrunc(t, a1t);
	_branchGenerated = irb.CreateCall(_returnFunction, {t});
	return _branchGenerated;
}

bool Capstone2LlvmIrTranslator_impl::isReturnFunction(llvm::Function* f) const
{
	return f == _returnFunction;
}

bool Capstone2LlvmIrTranslator_impl::isReturnFunctionCall(llvm::CallInst* c) const
{
	return c ? isReturnFunction(c->getCalledFunction()) : false;
}

void Capstone2LlvmIrTranslator_impl::generateBranchFunction()
{
	auto* ft = llvm::FunctionType::get(
			llvm::Type::getVoidTy(_module->getContext()),
			{llvm::Type::getIntNTy(_module->getContext(), getArchBitSize())},
			false);
	_branchFunction = llvm::Function::Create(
			ft,
			llvm::GlobalValue::LinkageTypes::ExternalLinkage,
			"",
			_module);
}

llvm::CallInst* Capstone2LlvmIrTranslator_impl::generateBranchFunctionCall(
		llvm::IRBuilder<>& irb,
		llvm::Value* t)
{
	auto* a1t = _branchFunction->getArgumentList().front().getType();
	t = irb.CreateSExtOrTrunc(t, a1t);
	_branchGenerated = irb.CreateCall(_branchFunction, {t});
	return _branchGenerated;
}

bool Capstone2LlvmIrTranslator_impl::isBranchFunction(llvm::Function* f) const
{
	return _branchFunction == f;
}

bool Capstone2LlvmIrTranslator_impl::isBranchFunctionCall(llvm::CallInst* c) const
{
	return c ? isBranchFunction(c->getCalledFunction()) : false;
}

void Capstone2LlvmIrTranslator_impl::generateCondBranchFunction()
{
	std::vector<llvm::Type*> params = {
			llvm::Type::getInt1Ty(_module->getContext()),
			llvm::Type::getIntNTy(_module->getContext(), getArchBitSize())};
	auto* ft = llvm::FunctionType::get(
			llvm::Type::getVoidTy(_module->getContext()),
			params,
			false);
	_condBranchFunction = llvm::Function::Create(
			ft,
			llvm::GlobalValue::LinkageTypes::ExternalLinkage,
			"",
			_module);
}

llvm::CallInst* Capstone2LlvmIrTranslator_impl::generateCondBranchFunctionCall(
		llvm::IRBuilder<>& irb,
		llvm::Value* cond,
		llvm::Value* t)
{
	auto* a1t = _condBranchFunction->getArgumentList().back().getType();
	t = irb.CreateSExtOrTrunc(t, a1t);
	_branchGenerated = irb.CreateCall(_condBranchFunction, {cond, t});
	return _branchGenerated;
}

bool Capstone2LlvmIrTranslator_impl::isCondBranchFunction(llvm::Function* f) const
{
	return _condBranchFunction == f;
}

bool Capstone2LlvmIrTranslator_impl::isCondBranchFunctionCall(llvm::CallInst* c) const
{
	return c ? isCondBranchFunction(c->getCalledFunction()) : false;
}

const csh& Capstone2LlvmIrTranslator_impl::getCapstoneEngine() const
{
	return _handle;
}

cs_arch Capstone2LlvmIrTranslator_impl::getArchitecture() const
{
	return _arch;
}

cs_mode Capstone2LlvmIrTranslator_impl::getBasicMode() const
{
	return _basicMode;
}

cs_mode Capstone2LlvmIrTranslator_impl::getExtraMode() const
{
	return _extraMode;
}

bool Capstone2LlvmIrTranslator_impl::hasDelaySlot(uint32_t id) const
{
	return false;
}

bool Capstone2LlvmIrTranslator_impl::hasDelaySlotTypical(uint32_t id) const
{
	return false;
}

bool Capstone2LlvmIrTranslator_impl::hasDelaySlotLikely(uint32_t id) const
{
	return false;
}

std::size_t Capstone2LlvmIrTranslator_impl::getDelaySlot(uint32_t id) const
{
	return 0;
}

llvm::Module* Capstone2LlvmIrTranslator_impl::getModule() const
{
	return _module;
}

llvm::GlobalVariable* Capstone2LlvmIrTranslator_impl::getAsm2LlvmMapGlobalVariable() const
{
	return _asm2llvmGv;
}

llvm::Function* Capstone2LlvmIrTranslator_impl::getCallFunction() const
{
	return _callFunction;
}

llvm::Function* Capstone2LlvmIrTranslator_impl::getReturnFunction() const
{
	return _returnFunction;
}

llvm::Function* Capstone2LlvmIrTranslator_impl::getBranchFunction() const
{
	return _branchFunction;
}

llvm::Function* Capstone2LlvmIrTranslator_impl::getCondBranchFunction() const
{
	return _condBranchFunction;
}

/**
 * @return Asm function associated with @p name, or @c nullptr if there is
 *         no such function.
 */
llvm::Function* Capstone2LlvmIrTranslator_impl::getAsmFunction(
		const std::string& name) const
{
	auto fIt = _asmFunctions.find(name);
	return fIt != _asmFunctions.end() ? fIt->second : nullptr;
}

/**
 * Get already existing asm functions associated with @p name, or if there
 * is no such function, create it using @p name and @p type, add it to asm
 * functions and return it.
 * @return Functions associated with @p insnId.
 */
llvm::Function* Capstone2LlvmIrTranslator_impl::getOrCreateAsmFunction(
		std::size_t insnId,
		const std::string& name,
		llvm::FunctionType* type)
{
	llvm::Function* fnc = getAsmFunction(name);
	if (fnc == nullptr)
	{
		fnc = llvm::Function::Create(
				type,
				llvm::GlobalValue::LinkageTypes::ExternalLinkage,
				name,
				_module);
		_asmFunctions[name] = fnc;
//		_asmFunctions[fnc->getName()] = fnc; // TODO ???
	}
	return fnc;
}

/**
 * The same as @c getOrCreateAsmFunction(std::size_t,std::string&, llvm::FunctionType*),
 * but function is created with zero parameters and @p retType return type.
 */
llvm::Function* Capstone2LlvmIrTranslator_impl::getOrCreateAsmFunction(
		std::size_t insnId,
		const std::string& name,
		llvm::Type* retType)
{
	return getOrCreateAsmFunction(
			insnId,
			name,
			llvm::FunctionType::get(retType, false));
}

/**
 * The same as @c getOrCreateAsmFunction(std::size_t,std::string&, llvm::FunctionType*),
 * but function is created with void return type and @p params parameters.
 *
 * TODO: This is not ideal, when used with only one argument (e.g. {i32}),
 * the llvm::Type* retType variant is used instead of this method.
 */
llvm::Function* Capstone2LlvmIrTranslator_impl::getOrCreateAsmFunction(
		std::size_t insnId,
		const std::string& name,
		llvm::ArrayRef<llvm::Type*> params)
{
	return getOrCreateAsmFunction(
			insnId,
			name,
			llvm::FunctionType::get(
					llvm::Type::getVoidTy(_module->getContext()),
					params,
					false));
}

/**
 * The same as @c getOrCreateAsmFunction(std::size_t,std::string&, llvm::FunctionType*),
 * but function type is created by this variant.
 */
llvm::Function* Capstone2LlvmIrTranslator_impl::getOrCreateAsmFunction(
		std::size_t insnId,
		const std::string& name,
		llvm::Type* retType,
		llvm::ArrayRef<llvm::Type*> params)
{
	return getOrCreateAsmFunction(
			insnId,
			name,
			llvm::FunctionType::get(retType, params, false));
}

llvm::GlobalVariable* Capstone2LlvmIrTranslator_impl::isRegister(llvm::Value* v) const
{
	auto it = _allLlvmRegs.find(llvm::dyn_cast_or_null<llvm::GlobalVariable>(v));
	return it != _allLlvmRegs.end() ? it->first : nullptr;
}

uint32_t Capstone2LlvmIrTranslator_impl::getCapstoneRegister(llvm::GlobalVariable* gv) const
{
	auto it = _allLlvmRegs.find(gv);
	return it != _allLlvmRegs.end() ? it->second : 0;
}

Capstone2LlvmIrTranslator_impl::TranslationResult Capstone2LlvmIrTranslator_impl::translate(
		const std::vector<uint8_t>& bytes,
		retdec::utils::Address a,
		llvm::IRBuilder<>& irb,
		bool stopOnBranch)
{
	TranslationResult res;

	cs_insn* insn = cs_malloc(_handle);

	const uint8_t* code = bytes.data();
	size_t size = bytes.size();
	uint64_t address = a;

	_branchGenerated = nullptr;
	_inCondition = false;

// TODO: hack, solve better.
	bool disasmRes = cs_disasm_iter(_handle, &code, &size, &address, insn);
	if (!disasmRes && _arch == CS_ARCH_MIPS && _basicMode == CS_MODE_MIPS32)
	{
		modifyBasicMode(CS_MODE_MIPS64);
		disasmRes = cs_disasm_iter(_handle, &code, &size, &address, insn);
		modifyBasicMode(CS_MODE_MIPS32);
	}

	while (disasmRes)
	{
		auto* a2l = generateSpecialAsm2LlvmInstr(irb, insn);
		if (res.first == nullptr)
		{
			res.first = a2l;
		}
		res.last = a2l;
		res.size = (insn->address + insn->size) - a;

		translateInstruction(insn, irb);

		// TODO: Optimize -- to make generation easier and nicer, some things
		// can be generated suboptimally. We should inspect every generated
		// ASM insruction and optimize some known patterns:
		//
		// 1. Load propagation:
		// a = load r
		// ... use a, not change r, no fnc call, etc.
		// b = load r
		// ... use b -> replace by a, remove b
		//
		// 2. Conversions:
		// a = cast b
		// ... use a
		// c = cast b
		// ... use c -> replace by a, remove c
		//
		// 3. Unused values (e.g. from loadOpBinary() where only one op used):
		// a = load x
		// ... a unused
		//
		// 4. Values used only for their type (e.g. op0 load in translateMov()):
		// a = load x
		// b = load y
		// c = convert b to a.type
		// store c x
		//
		// etc.

		if (_branchGenerated && stopOnBranch)
		{
			res.branchCall = _branchGenerated;
			res.inCondition = _inCondition;
			return res;
		}

		insn = cs_malloc(_handle);

// TODO: hack, solve better.
		disasmRes = cs_disasm_iter(_handle, &code, &size, &address, insn);
		if (!disasmRes && _arch == CS_ARCH_MIPS && _basicMode == CS_MODE_MIPS32)
		{
			modifyBasicMode(CS_MODE_MIPS64);
			disasmRes = cs_disasm_iter(_handle, &code, &size, &address, insn);
			modifyBasicMode(CS_MODE_MIPS32);
		}
	}

	// TODO: nothing translated, or translation ended far before end
	// -> throw || or signal this to user (decoder).

	cs_free(insn, 1);

	return res;
}

llvm::GlobalVariable* Capstone2LlvmIrTranslator_impl::createRegister(
		uint32_t r,
		llvm::GlobalValue::LinkageTypes lt,
		llvm::Constant* initializer)
{
	auto* rt = getRegisterType(r);
	if (initializer == nullptr
			&& lt != llvm::GlobalValue::LinkageTypes::ExternalLinkage)
	{
		if (auto* it = llvm::dyn_cast<llvm::IntegerType>(rt))
		{
			initializer = llvm::ConstantInt::get(it, 0);
		}
		else if (rt->isFloatingPointTy())
		{
			initializer = llvm::ConstantFP::get(rt, 0);
		}
		else
		{
			throw Capstone2LlvmIrError("Unhandled register type.");
		}
	}

	auto* gv = new llvm::GlobalVariable(
			*_module,
			rt,
			false, // isConstant
			lt,
			initializer,
			getRegisterName(r));

	if (gv == nullptr)
	{
		throw Capstone2LlvmIrError("Memory allocation error.");
	}

	_allLlvmRegs[gv] = r;

	return gv;
}

std::string Capstone2LlvmIrTranslator_impl::getRegisterName(uint32_t r) const
{
	auto fIt = _reg2name.find(r);
	if (fIt == _reg2name.end())
	{
		if (auto* n = cs_reg_name(_handle, r))
		{
			return n;
		}
		else
		{
			throw Capstone2LlvmIrError(
					"Missing name for register number: " + std::to_string(r));
		}
	}
	else
	{
		return fIt->second;
	}
}

uint32_t Capstone2LlvmIrTranslator_impl::getRegisterBitSize(uint32_t r) const
{
	auto* rt = getRegisterType(r);
	if (auto* it = llvm::dyn_cast<llvm::IntegerType>(rt))
	{
		return it->getBitWidth();
	}
	else if (rt->isHalfTy())
	{
		return 16;
	}
	else if (rt->isFloatTy())
	{
		return 32;
	}
	else if (rt->isDoubleTy())
	{
		return 64;
	}
	else if (rt->isX86_FP80Ty())
	{
		return 80;
	}
	else if (rt->isFP128Ty())
	{
		return 128;
	}
	else
	{
		throw Capstone2LlvmIrError(
				"Unhandled type of register number: " + std::to_string(r));
	}
}

uint32_t Capstone2LlvmIrTranslator_impl::getRegisterByteSize(uint32_t r) const
{
	return getRegisterBitSize(r) / 8;
}

llvm::Type* Capstone2LlvmIrTranslator_impl::getRegisterType(uint32_t r) const
{
	auto fIt = _reg2type.find(r);
	if (fIt == _reg2type.end())
	{
		assert(false);
		throw Capstone2LlvmIrError(
				"Missing type for register number: " + std::to_string(r));
	}
	return fIt->second;
}

/**
 * If this becomes a bottleneck it could be optimized using cache.
 * Either local cache here initialized as this method is used,
 * or global cache initialized at register generation.
 * @return LLVM global for the specified register, or @c nullptr if not found.
 */
llvm::GlobalVariable* Capstone2LlvmIrTranslator_impl::getRegister(uint32_t r)
{
	auto rn = getRegisterName(r);
	return _module->getNamedGlobal(rn);
}

/**
 * Generate if-then statement at the current insert point of @p irb builder.
 * @code{.cpp}
	if (cond) {
	  // body
	}
	// after
 * @endcode
 * @param cond Value used as condition in @c if() statement.
 * @param irb  Reference to IR builder. After if-then is generated,
 *             irb's insert point is set to first instruction after the
 *             statement.
 * @return IR builder whose insert point is set to if-then body BB's
 *         terminator instruction. Use this builder to fill the body.
 */
llvm::IRBuilder<> Capstone2LlvmIrTranslator_impl::generateIfThen(
		llvm::Value* cond,
		llvm::IRBuilder<>& irb)
{
	return _generateIfThen(cond, irb, false);
}

/**
 * Same as @c generateIfThen() but if @p cond is @c true, body is skipped:
 * @code{.cpp}
	if (!cond) {
	  // body
	}
	// after
 * @endcode
 */
llvm::IRBuilder<> Capstone2LlvmIrTranslator_impl::generateIfNotThen(
		llvm::Value* cond,
		llvm::IRBuilder<>& irb)
{
	return _generateIfThen(cond, irb, true);
}

llvm::IRBuilder<> Capstone2LlvmIrTranslator_impl::_generateIfThen(
		llvm::Value* cond,
		llvm::IRBuilder<>& irb,
		bool reverse)
{
	if (auto* ci = llvm::dyn_cast<llvm::ConstantInt>(cond))
	{
		if (ci->isZero())
		{
			if (reverse)
			{
				// llvm::BranchInst::Create(after, body, cond, ipBb->getTerminator());
				// cond == false -> never jump to after -> body always executed
				return irb;
			}
			else
			{
				// llvm::BranchInst::Create(body, after, cond, ipBb->getTerminator());
				// todo: cond == false -> never jump to body -> body never executed
			}
		}
		else
		{
			if (reverse)
			{
				// llvm::BranchInst::Create(after, body, cond, ipBb->getTerminator());
				// todo: cond == true -> always jump to after -> body never executed
			}
			else
			{
				// llvm::BranchInst::Create(body, after, cond, ipBb->getTerminator());
				// cond == true -> always jump to body -> body always executed
				return irb;
			}
		}
	}

	auto* ipBb = irb.GetInsertBlock();
	auto ipIt = irb.GetInsertPoint();
	assert(ipIt != ipBb->end());
	llvm::Instruction* ip = &(*ipIt);

	auto* body = ipBb->splitBasicBlock(ip);
	auto* after = body->splitBasicBlock(ip);

	if (reverse)
	{
		llvm::BranchInst::Create(after, body, cond, ipBb->getTerminator());
	}
	else
	{
		llvm::BranchInst::Create(body, after, cond, ipBb->getTerminator());
	}
	ipBb->getTerminator()->eraseFromParent();
	irb.SetInsertPoint(ip);

	return llvm::IRBuilder<>(body->getTerminator());
}

/**
 * Generate if-then-else statement at the current insert point of @p irb builder.
 * @code{.cpp}
	if (cond) {
	  // bodyIf
	} else {
	  // bodyElse
	}
	// after
 * @endcode
 * @param cond Value used as condition in @c if() statement.
 * @param irb  Reference to IR builder. After if-then-else is
 *             generated, irb's insert point is set to first instruction after
 *             the statement.
 * @return Pair of IR builders whose insert points are set to if-then-else's
 *         bodyIf (first) and bodyElse (second) terminator instructions.
 *         Use these builders to fill the bodies.
 */
std::pair<llvm::IRBuilder<>, llvm::IRBuilder<>> Capstone2LlvmIrTranslator_impl::generateIfThenElse(
		llvm::Value* cond,
		llvm::IRBuilder<>& irb)
{
	auto* ipBb = irb.GetInsertBlock();
	auto ipIt = irb.GetInsertPoint();
	assert(ipIt != ipBb->end());
	llvm::Instruction* ip = &(*ipIt);

	auto* bodyIf = ipBb->splitBasicBlock(ip);
	auto* bodyElse = bodyIf->splitBasicBlock(ip);
	auto* after = bodyElse->splitBasicBlock(ip);

	llvm::BranchInst::Create(bodyIf, bodyElse, cond, ipBb->getTerminator());
	ipBb->getTerminator()->eraseFromParent();

	llvm::BranchInst::Create(after, bodyIf->getTerminator());
	bodyIf->getTerminator()->eraseFromParent();

	irb.SetInsertPoint(ip);

	return std::make_pair(
			llvm::IRBuilder<>(bodyIf->getTerminator()),
			llvm::IRBuilder<>(bodyElse->getTerminator()));
}

/**
 * Generate while statement at the current insert point of @p irb builder.
 * @code{.cpp}
	// before
	while (cond) {
	  // body
	}
	// after
 * @endcode
 * @param branch Reference to a branch instruction pointer that will be filled
 *               with a while's conditional branch, whose condition is set to
 *               @c true (infinite loop). Use before IR builder to generate
 *               condition and @c llvm::BranchInst::setCondition() to set it to
 *               whis branch.
 * @param irb    Reference to IR builder. After while is generated, irb's insert
 *               point is set to first instruction after the statement.
 * @return Pair of IR builders whose insert points are set to before BB's and
 *         while body BB's terminator instructions.
 *         Use these builders to fill while's condition and body.
 */
std::pair<llvm::IRBuilder<>, llvm::IRBuilder<>> Capstone2LlvmIrTranslator_impl::generateWhile(
		llvm::BranchInst*& branch,
		llvm::IRBuilder<>& irb)
{
	auto* ipBb = irb.GetInsertBlock();
	auto ipIt = irb.GetInsertPoint();
	assert(ipIt != ipBb->end());
	llvm::Instruction* ip = &(*ipIt);

	auto* before = ipBb->splitBasicBlock(ip);
	auto* body = before->splitBasicBlock(ip);
	auto* after = body->splitBasicBlock(ip);

	branch = llvm::BranchInst::Create(body, after, irb.getTrue(), before->getTerminator());
	before->getTerminator()->eraseFromParent();

	llvm::BranchInst::Create(before, body->getTerminator());
	body->getTerminator()->eraseFromParent();

	irb.SetInsertPoint(ip);

	return std::make_pair(
			llvm::IRBuilder<>(before->getTerminator()),
			llvm::IRBuilder<>(body->getTerminator()));
}

/**
 * TODO: Probably move to abstract class.
 */
llvm::Value* Capstone2LlvmIrTranslator_impl::genValueNegate(llvm::IRBuilder<>& irb, llvm::Value* val)
{
	return irb.CreateXor(val, llvm::ConstantInt::getSigned(val->getType(), -1));
}

llvm::Type* Capstone2LlvmIrTranslator_impl::getIntegerTypeFromByteSize(unsigned sz)
{
	auto& ctx = _module->getContext();
	switch (sz)
	{
		case 1: return llvm::Type::getInt8Ty(ctx);
		case 2: return llvm::Type::getInt16Ty(ctx);
		case 4: return llvm::Type::getInt32Ty(ctx);
		case 6: return llvm::Type::getIntNTy(ctx, 48);
		case 8: return llvm::Type::getInt64Ty(ctx);
		default: throw Capstone2LlvmIrError("should not be possible");
	}
}

llvm::Type* Capstone2LlvmIrTranslator_impl::getFloatTypeFromByteSize(unsigned sz)
{
	auto& ctx = _module->getContext();
	switch (sz)
	{
		case 2: return llvm::Type::getHalfTy(ctx);
		case 4: return llvm::Type::getFloatTy(ctx);
		case 8: return llvm::Type::getDoubleTy(ctx);
		case 10: return llvm::Type::getX86_FP80Ty(ctx);
		case 16: return llvm::Type::getFP128Ty(ctx);
		default: throw Capstone2LlvmIrError("should not be possible");
	}
}

} // namespace capstone2llvmir
} // namespace retdec
