/*
 * LLVM backend for Tiny Code Generator for QEMU
 *
 * Copyright (c) 2010 Volodymyr Kuznetsov
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef TCG_LLVM_H
#define TCG_LLVM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tcg.h"

/*****************************/
/* Functions for QEMU c code */

struct TranslationBlock;
struct TCGLLVMContext;

extern struct TCGLLVMContext* tcg_llvm_ctx;

struct TCGLLVMRuntime {
    // NOTE: The order of these are fixed !
    uint64_t helper_ret_addr;
    uint64_t helper_call_addr;
    uint64_t helper_regs[3];
    // END of fixed block

#ifdef CONFIG_S2E
    /* run-time tb linking mechanism */
    uint8_t goto_tb;
#endif

#ifndef CONFIG_S2E
    TranslationBlock *last_tb;
    uint64_t last_opc_index;
    uint64_t last_pc;
#endif
};

extern struct TCGLLVMRuntime tcg_llvm_runtime;

struct TCGLLVMContext* tcg_llvm_initialize(void);
void tcg_llvm_close(struct TCGLLVMContext *l);

void tcg_llvm_tb_alloc(struct TranslationBlock *tb);
void tcg_llvm_tb_free(struct TranslationBlock *tb);

void tcg_llvm_gen_code(struct TCGLLVMContext *l, struct TCGContext *s,
                       struct TranslationBlock *tb);
const char* tcg_llvm_get_func_name(struct TranslationBlock *tb);

uintptr_t tcg_llvm_qemu_tb_exec(struct TranslationBlock *tb,
                            void* volatile* saved_AREGs);

#ifndef CONFIG_S2E
int tcg_llvm_search_last_pc(struct TranslationBlock *tb, uintptr_t searched_pc);
#endif

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

/***********************************/
/* External interface for C++ code */

namespace llvm {
    class Function;
    class LLVMContext;
    class Module;
    class ModuleProvider;
    class ExecutionEngine;
    class FunctionPassManager;
}

class TCGLLVMContextPrivate;
class TCGLLVMContext
{
private:
    TCGLLVMContextPrivate* m_private;

public:
    TCGLLVMContext();
    ~TCGLLVMContext();

    llvm::LLVMContext& getLLVMContext();

    llvm::Module* getModule();
    llvm::ModuleProvider* getModuleProvider();

    llvm::ExecutionEngine* getExecutionEngine();

    void deleteExecutionEngine();
    llvm::FunctionPassManager* getFunctionPassManager() const;

#ifdef CONFIG_S2E
    /** Called after linking all helper libraries */
    void initializeHelpers();
#endif

    void generateCode(struct TCGContext *s,
                      struct TranslationBlock *tb);
};

#endif

#endif

