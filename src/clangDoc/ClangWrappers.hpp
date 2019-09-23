#pragma once

#include <exception>
#include <algorithm>
#include <cassert>
#include <memory>
#include <cstdio>

#include <clang-c/Index.h>

#include "util.hpp"

namespace clangw {
  struct clangerr;

  class Index;
  class TranslationUnit;
  class String;
  struct Kind;
  struct Cursor;

  constexpr const char* describeClangError(const int code);
}

namespace clangw {
  struct clangerr : public std::exception {
    public:
      explicit clangerr(const int code) noexcept :
        code_(code),
        msg_(describeClangError(code))
      {}
      int code() const noexcept {
        return code_;
      }
      const char* what() const noexcept override;

    private:
      int code_;
      const char* msg_;
  };

  class String {
    public:
      String(CXString s) :
        s_(s)
      {
        assert(s_.data != nullptr);
      }
      ~String() {
        if (s_.data != nullptr)
          clang_disposeString(s_);
      }

      const char* cstr() {
        return clang_getCString(unsafeRaw());
      }

      mimpl_cpp_nocopy(String)
      mimpl_cpp_copy_and_swap(String) {
        using std::swap;
        swap(a.s_, b.s_);
      }

      CXString unsafeRaw() {
        assert(s_.data != nullptr);
        return s_;
      }

    private:
      String() :
        s_()
      {
        s_.data = nullptr;
      }

      CXString s_;
  };

  namespace cursor {
    struct Kind {
      public:
        Kind(CXCursorKind k) :
          raw(k)
        {}

        // @|url https://clang.llvm.org/doxygen/group__CINDEX__CURSOR__MANIP.html#ga660aa4846fce0a54e20073ab6a5465a0
        bool declaration() {
          return clang_isDeclaration(raw) != 0;
        }
        // @|url https://clang.llvm.org/doxygen/group__CINDEX__CURSOR__MANIP.html#gaf1bf500b9ada62671b53831d023387ba
        bool reference() {
          return clang_isReference(raw) != 0;
        }
        // @|url https://clang.llvm.org/doxygen/group__CINDEX__CURSOR__MANIP.html#ga8e537f2f251a92a799d6cc8459614d42
        bool expression() {
          return clang_isExpression(raw) != 0;
        }
        // @|url https://clang.llvm.org/doxygen/group__CINDEX__CURSOR__MANIP.html#ga33c9d1d9cf46a316160f68356608773a
        bool statement() {
          return clang_isStatement(raw) != 0;
        }
        // @|url https://clang.llvm.org/doxygen/group__CINDEX__CURSOR__MANIP.html#ga204227c8c254c568ef6d577ffcf8d3e5
        bool attribute() {
          return clang_isAttribute(raw) != 0;
        }
        // @|url https://clang.llvm.org/doxygen/group__CINDEX__CURSOR__MANIP.html#ga86d525c70189f9d04caf5aa59121c384
        bool valid() {
          return clang_isInvalid(raw) == 0;
        }
        // @|url https://clang.llvm.org/doxygen/group__CINDEX__CURSOR__MANIP.html#gaa25633b28eb4799da0952e9beb03799b
        bool translationUnit() {
          return clang_isTranslationUnit(raw) != 0;
        }
        // @|url https://clang.llvm.org/doxygen/group__CINDEX__CURSOR__MANIP.html#gab2c617ece02f6e3f41e403b8c7d85bce
        bool preprocessing() {
          return clang_isPreprocessing(raw) != 0;
        }
        // @|url https://clang.llvm.org/doxygen/group__CINDEX__CURSOR__MANIP.html#ga838c465c6d1e681d2469eaa5adffcceb
        bool exposed() {
          return clang_isUnexposed(raw) == 0;
        }

        // @|url https://clang.llvm.org/doxygen/group__CINDEX__DEBUG.html#ga7a4eecfc1b343568cb9ea447cbde08a8
        String spelling() {
          return String(clang_getCursorKindSpelling(raw));
        }

        CXCursorKind raw;
    };
  }

  using CursorVisitor = CXChildVisitResult (Cursor c, Cursor parent, CXClientData data);
  struct Cursor {
    public:
      Cursor(CXCursor c) :
        raw(c)
      {}

      // true if the visitor returned CXChildVisit_Break
      // @|url https://clang.llvm.org/doxygen/group__CINDEX__CURSOR__TRAVERSAL.html#ga5d0a813d937e1a7dcc35f206ad1f7a91
      bool visitChildren(CursorVisitor* vis) {
        return clang_visitChildren(raw, [](CXCursor c, CXCursor parent, CXClientData d) {
          CursorVisitor* v = reinterpret_cast<CursorVisitor*>(d);
          return v(Cursor(c), Cursor(parent), nullptr);
        }, reinterpret_cast<void*>(vis)) != 0;
      }

      // @|url https://clang.llvm.org/doxygen/group__CINDEX__CURSOR__MANIP.html#ga018aaf60362cb751e517d9f8620d490c
      cursor::Kind kind() {
        return cursor::Kind(clang_getCursorKind(raw));
      }
      // @|url https://clang.llvm.org/doxygen/group__CINDEX__CURSOR__MANIP.html#ga359dae25aa1a71176a5e33f3c7ee1740
      CXLinkageKind linkage() {
        return clang_getCursorLinkage(raw);
      }
      // @|url https://clang.llvm.org/doxygen/group__CINDEX__CURSOR__MANIP.html#ga935b442bd6bde168cf354b7629b471d8
      CXVisibilityKind visibility() {
        return clang_getCursorVisibility(raw);
      }
      // @|url https://clang.llvm.org/doxygen/group__CINDEX__CURSOR__MANIP.html#gab44e2a565fa40a0e0fc0f130f618a9b5
      CXAvailabilityKind availability() {
        return clang_getCursorAvailability(raw);
      }
      // @|url https://clang.llvm.org/doxygen/group__CINDEX__CURSOR__MANIP.html#ga3729a27620b08e32e331a6c168e707b3
      CXLanguageKind language() {
        return clang_getCursorLanguage(raw);
      }
      // @|url https://clang.llvm.org/doxygen/group__CINDEX__CURSOR__MANIP.html#ga524e1bd046dfb581484ec50e8f22ae7a
      CXTLSKind tlsKind() {
        return clang_getCursorTLSKind(raw);
      }
      // @|url https://clang.llvm.org/doxygen/group__CINDEX__TYPES.html#gaae5702661bb1f2f93038051737de20f4
      CXType type() {
        return clang_getCursorType(raw);
      }
      // @|url https://clang.llvm.org/doxygen/group__CINDEX__CODE__COMPLET.html#ga403bcb1ebc70f1ec9e19543d76685f43
      CXCompletionString completion() {
        return clang_getCursorCompletionString(raw);
      }

      // @|url https://clang.llvm.org/doxygen/group__CINDEX__CURSOR__MANIP.html#gabc327b200d46781cf30cb84d4af3c877
      Cursor semanticParent() {
        return Cursor(clang_getCursorSemanticParent(raw));
      }
      // @|url https://clang.llvm.org/doxygen/group__CINDEX__CURSOR__MANIP.html#gace7a423874d72b3fdc71d6b0f31830dd
      Cursor lexicalParent() {
        return Cursor(clang_getCursorLexicalParent(raw));
      }
      // @|url https://clang.llvm.org/doxygen/group__CINDEX__CURSOR__XREF.html#gabf059155921552e19fc2abed5b4ff73a
      Cursor referenced() {
        return Cursor(clang_getCursorReferenced(raw));
      }
      // @|url https://clang.llvm.org/doxygen/group__CINDEX__CURSOR__XREF.html#gafcfbec461e561bf13f1e8540bbbd655b
      Cursor definition() {
        return Cursor(clang_getCursorDefinition(raw));
      }

      // @|url https://clang.llvm.org/doxygen/group__CINDEX__CURSOR__XREF.html#ga51679cb755bbd94cc5e9476c685f2df3
      String usr() {
        return String(clang_getCursorUSR(raw));
      }
      // @|url https://clang.llvm.org/doxygen/group__CINDEX__CURSOR__XREF.html#gaad1c9b2a1c5ef96cebdbc62f1671c763
      String spelling() {
        return String(clang_getCursorSpelling(raw));
      }
      // @|url https://clang.llvm.org/doxygen/group__CINDEX__CURSOR__XREF.html#gac3eba3224d109a956f9ef96fd4fe5c83
      String displayName() {
        return String(clang_getCursorDisplayName(raw));
      }
      // @|url https://clang.llvm.org/doxygen/group__CINDEX__CURSOR__XREF.html#gab9d561cc237ce0d8bfbab80cdd5be216
      String prettyPrint() {
        return String(clang_getCursorPrettyPrinted(raw, prettyPrintingPolicy()));
      }
      // @|url https://clang.llvm.org/doxygen/group__CINDEX__CURSOR__XREF.html#gab9d561cc237ce0d8bfbab80cdd5be216
      String prettyPrint(CXPrintingPolicy policy) {
        return String(clang_getCursorPrettyPrinted(raw, policy));
      }
      // @|url https://clang.llvm.org/doxygen/group__CINDEX__CURSOR__XREF.html#gaae83c013276d1fff6475566a23d9fffd
      CXPrintingPolicy prettyPrintingPolicy() {
        return clang_getCursorPrintingPolicy(raw);
      }

      // the following methods only returns a valid type for functions and methods

      // @|url https://clang.llvm.org/doxygen/group__CINDEX__TYPES.html#ga6995a2d6352e7136868574b299005a63
      CXType returnType() {
        return clang_getCursorResultType(raw);
      }
      // @|url https://clang.llvm.org/doxygen/group__CINDEX__TYPES.html#gae9536e898e50a2d95975d9be0617aaa8
      CXCursor_ExceptionSpecificationKind exceptionSpec() {
        return static_cast<CXCursor_ExceptionSpecificationKind>(clang_getCursorExceptionSpecificationType(raw));
      }

      // @|url https://clang.llvm.org/doxygen/group__CINDEX__CURSOR__SOURCE.html#gada3d3cbd3a3e83ff64f992617318dfb1
      CXSourceLocation location() {
        return clang_getCursorLocation(raw);
      }
      // @|url https://clang.llvm.org/doxygen/group__CINDEX__CURSOR__SOURCE.html#ga79f6544534ab73c78a8494c4c0bc2840
      CXSourceRange extent() {
        return clang_getCursorExtent(raw);
      }

      CXCursor raw;
  };

  // wrap the clang data structures with RAII classes
  // also member functions save the trouble of passing stuff around

  // assumes that @|{CXIndex is a pointer type
  // @|url https://clang.llvm.org/doxygen/group__CINDEX.html#gae039c2574bfd75774ca7a9a3e55910cb
  class Index : public std::enable_shared_from_this<Index> {
    public:
      // @|url https://clang.llvm.org/doxygen/group__CINDEX.html#ga166ab73b14be73cbdcae14d62dbab22a
      ~Index() noexcept {
        if (i_ != nullptr)
          clang_disposeIndex(unsafeRaw());
      }

      // diagnostics = errors + warnings
      // @|url https://clang.llvm.org/doxygen/group__CINDEX.html#ga51eb9b38c18743bf2d824c6230e61f93
      Index(const bool unknownWhatPCH, const bool printDiagnostics) :
        i_(clang_createIndex(unknownWhatPCH, printDiagnostics))
      {
        assert(i_ != nullptr);
      }
      mimpl_cpp_nocopy(Index)
      mimpl_cpp_copy_and_swap(Index) {
        using std::swap;
        swap(a.i_, b.i_);
      }

      mimpl_any_const_getter(CXIndex, Index, unsafeRaw,) {
        assert(i_ != nullptr);
        return i_;
      }

      TranslationUnit makeTranslationUnit(
        const char* path,
        const char* const * argv, const int argc,
        CXUnsavedFile* unsavedFiles, const unsigned int unsavedFilesN,
        const unsigned int flags = CXTranslationUnit_None
      );

    private:
      Index() :
        i_(nullptr)
      {}

      CXIndex i_;
  };

  // assumes that @|{CXTranslationUnit is a pointer type
  // @|url https://clang.llvm.org/doxygen/group__CINDEX.html#gacdb7815736ca709ce9a5e1ec2b7e16ac
  class TranslationUnit {
    public:
      // @|url https://clang.llvm.org/doxygen/group__CINDEX__TRANSLATION__UNIT.html#gaee753cb0036ca4ab59e48e3dff5f530a
      ~TranslationUnit() {
        if (u_ != nullptr)
          clang_disposeTranslationUnit(unsafeRaw());
      }

      mimpl_cpp_nocopy(TranslationUnit)
      mimpl_cpp_copy_and_swap(TranslationUnit) {
        using std::swap;
        swap(a.u_, b.u_);
      }

      mimpl_any_const_getter(CXTranslationUnit, TranslationUnit, unsafeRaw,) {
        assert(u_ != nullptr);
        return u_;
      }

      // @|url https://clang.llvm.org/doxygen/group__CINDEX__CURSOR__MANIP.html#gaec6e69127920785e74e4a517423f4391
      Cursor getRootCursor() {
        return Cursor(clang_getTranslationUnitCursor(unsafeRaw()));
      }

    private:
      friend class Index;
      TranslationUnit() :
        i_(nullptr),
        u_(nullptr)
      {}
      // todo: @|{CXUnsavedFile
      // todo: @|{.flags
      // @|url https://clang.llvm.org/doxygen/group__CINDEX__TRANSLATION__UNIT.html#ga494de0e725c5ae40cbdea5fa6081027d
      TranslationUnit(
        std::shared_ptr<Index>&& i, const char* path,
        const char* const * argv, const int argc,
        CXUnsavedFile* unsavedFiles, const unsigned int unsavedFilesN,
        const unsigned int flags = CXTranslationUnit_None
      ) :
        i_(i),
        u_(nullptr)
      {
        CXErrorCode err = clang_parseTranslationUnit2(i->unsafeRaw(), path, argv, argc, unsavedFiles, unsavedFilesN, flags, &u_);
        if (err != CXError_Success)
          throw clangerr(err);
      }

      std::shared_ptr<Index> i_;
      CXTranslationUnit u_;
  };

  constexpr const char* describeClangError(const int code) {
    assert(code != CXError_Success);
    if (code == CXError_Failure)
      return "A generic error code, no further details are available.";
    if (code == CXError_Crashed)
      return "libclang crashed while performing the requested operation.";
    if (code == CXError_InvalidArguments)
      return "The function detected that the arguments violate the function contract.";
    if (code == CXError_ASTReadError)
      return "An AST deserialization error has occurred.";
    return "Unknown error.";
  }
}
