#include "swigmod.h"
#include "cparse.h"
#include <ctype.h>
#include "../DoxygenTranslator/src/JavaDocConverter.h"

class OBJECTIVEC:public Language {
private:
    /* Files and file sections containing generated code. */
    File *f_wrap_h;		// Wrapper header file
    File *f_wrap_mm;		// Wrapper source file
    File *f_proxy_h;		// Proxy header file
    File *f_proxy_mm;		// Proxy source file
    File *f_runtime;
    File *f_header;		/* General DOH objects used for holding the sections of wrapper source */
    File *f_wrappers;
    File *f_init;
    File *f_directors;
    File *f_directors_h;

    /* Strings temporarily holding the generated C++ code. */
    //String *wrap_h_code;		// Code to be written in wrap_h.
    String *wrap_mm_code;		// Code to be written in wrap_mm.
    String *proxy_h_code;		// Code to be written in proxy_h.
    String *proxy_mm_code;	// Code to be written in proxy_mm.
    String *swigtypes_h_code;	// Code for Objective-C typewrapper classes header.
    String *swigtypes_mm_code;	// Code for Objective-C typewrapper classes implementation.
    
    
    /* Various flags controlling the code generation. */
    bool proxy_flag;		// flag: determine should the proxy files be generated or not
    
    /* State variables which indicate what is being wrapped at the moment. */
    bool member_variable_flag;	// flag: wrapping member variables
    bool static_member_variable_flag;	// flag: wrapping static member variables
    bool global_variable_flag;	// flag: wrapping global variables
    bool global_func_flag;	// flag: wrapping global functions
    bool static_member_func_flag;	// flag: wrapping static member functions
    bool member_func_flag;	// flag: wrapping member functions
    bool member_constant_flag;	// flag: wrapping member constants
    bool doxygen;         //flag for converting found doxygen
    bool comment_creation_chatter; //flag for getting information about where comments were created in objc.cxx
    
    /* Helper strings used in the code */
    String *current_class_name;	// String representing name of the current class.
    String *current_class_type;	// Current class when used as a type. This represents the complete name of the class including the scope prefix
    String *variable_name;	// String representing the current variable name.
    String *proxyfuncname; // String representing the current memberfunction name.
    
    /* ObjectiveC data for the current proxy class:
     * These strings are mainly used to temporarily accumulate code from the
     * various member handling functions while a single class is processed and are
     * no longer relevant once that class has been finished, i.e. after
     * classHandler() has returned. */
    String *proxy_class_name;	// The unqualified name of the current proxy class.
    String *proxy_class_qname;	// The name of the current proxy class, qualified with the name of the namespace it is in, if any.
    // TODO: Add this when nspaces are handled. Not now!
    
    String *proxy_class_decl_code;	// The proxy class declaration code.This goes in the proxy_h file if proxy_flag is true.
    String *proxy_class_defn_code;	// The proxy class definition code.This goes in the proxy_mm file if proxy_flag is true.
    String *proxy_class_decl_imports;	// The import directives for the current proxy class. This goes in the proxy_h file if proxy_flag is true.
    String *proxy_class_defn_imports;	// The import directives for the current proxy class. This goes in the proxy_mm file if proxy_flag is true.
    
    String *proxy_class_enums_code;	// Code for enumerations nested in the current proxy class. Is emitted globally and earlier
    // than the rest of the body to work around forward referencing-issues.
    
    String *proxy_class_function_decls;	// Code for the proxy class member functions declaration.
    String *proxy_class_function_defns;	// Code for the proxy class member functions definition.
    String *proxy_global_function_decls;	// Code for the proxy class member functions declaration.
    String *proxy_global_function_defns;	// Code for the proxy class member functions definition.
    String *destrcutor_call;	// Contains an ObjectiveC call to the function wrapping the C++  destructor of the
    // current class (if there is a public C++ destructor).
    String *structuralComments;
    
    
    /* SWIG types data: Collects information about encountered types SWIG does not know about (e.g.
     * incomplete types). This is used later to generate type wrapper proxy.
     * classes for the unknown types. */
    Hash *unknown_types;
    
    /* Strings used at different places in the code. */
    static const char *const usage;	// Usage message
    const String *empty_string;	// Empty string used at different places in the code
    
    
public:
    OBJECTIVEC():
    f_wrap_h(NULL),
    f_wrap_mm(NULL),
    f_proxy_h(NULL),
    f_proxy_mm(NULL),
    f_runtime(NULL),
    f_header(NULL),
    f_wrappers(NULL),
    f_init(NULL),
    //wrap_h_code(NULL),
    f_directors(NULL),
    f_directors_h(NULL),
    wrap_mm_code(NULL),
    proxy_h_code(NULL),
    proxy_mm_code(NULL),
    swigtypes_h_code(NULL),
    swigtypes_mm_code(NULL),
    proxy_flag(true),
    member_variable_flag(false),
    static_member_variable_flag(false),
    global_variable_flag(false),
    global_func_flag(false),
    static_member_func_flag(false),
    member_func_flag(false),
    member_constant_flag(false),
    doxygen(false),
    comment_creation_chatter(false),
    current_class_name(NULL),
    current_class_type(NULL),
    variable_name(NULL),
    proxyfuncname(NULL),
    proxy_class_name(NULL),
    proxy_class_qname(NULL),
    proxy_class_decl_code(NULL),
    proxy_class_defn_code(NULL),
    proxy_class_decl_imports(NULL),
    proxy_class_defn_imports(NULL),
    proxy_class_enums_code(NULL),
    proxy_class_function_decls(NULL),
    proxy_class_function_defns(NULL),
    proxy_global_function_decls(NULL),
    proxy_global_function_defns(NULL),
    destrcutor_call(NULL),
    structuralComments(NULL),
    unknown_types(NULL),
    empty_string(NewString("")) {
        director_multiple_inheritance = 0;
        director_language = 1;
    }
    
    ~OBJECTIVEC() {
        if (doxygen)
          delete doxygenTranslator;
    }
    
    String *directorClassName(Node *n);
    
    virtual void main(int argc, char *argv[]);
    virtual int top(Node *n);

    String *getOverloadedName(Node *n);
    
    /* Function handlers */
    virtual int functionHandler(Node *n);
    virtual int globalfunctionHandler(Node *n);
    virtual int memberfunctionHandler(Node *n);
    virtual int staticmemberfunctionHandler(Node *n);
    virtual int callbackfunctionHandler(Node *n);
    
    /* Variable handlers */
    virtual int variableHandler(Node *n);
    virtual int globalvariableHandler(Node *n);
    virtual int membervariableHandler(Node *n);
    virtual int staticmembervariableHandler(Node *n);
    
    /* C++ handlers */
    virtual int memberconstantHandler(Node *n);
    virtual int constructorHandler(Node *n);
    virtual int copyconstructorHandler(Node *n);
    virtual int destructorHandler(Node *n);
    virtual int classHandler(Node *n);
    
    /* C/C++ Parsing */
    virtual int enumDeclaration(Node *n);
    virtual int enumvalueDeclaration(Node *n);
    
    /* Low-level code generation */
    virtual int constantWrapper(Node *n);
    virtual int variableWrapper(Node *n);
    virtual int functionWrapper(Node *n);
    virtual int nativeWrapper(Node *n);
    
    /* Proxy class code generators */
    void emitProxyGlobalFunctions(Node *n);
    void emitProxyClassFunction(Node *n);
    void emitProxyClassConstructor(Node *n);
    void emitProxyClass(Node *n);
    void emitTypeWrapperClass(String *classname, SwigType *type);
    
    /* Pragma directives */
    virtual int pragmaDirective(Node *n);

    /* Directors */
    void emitDirectorExtraMethods(Node *n);
    virtual int classDirectorMethod(Node *n, Node *parent, String *super);
    virtual int classDirectorInit(Node *n);
    virtual int classDirectorEnd(Node *n);
    virtual int classDirectorConstructor(Node *n);
    virtual int classDirectorDefaultConstructor(Node *n);
    virtual int classDirectorDestructor(Node *n);
    virtual int classDirectorDisown(Node *n);
    virtual bool extraDirectorProtectedCPPMethodsRequired() const;
    void Objc_director_declaration(Node *n);

    /* Helper functions */
    bool substituteClassname(String *tm, SwigType *pt);
    void substituteClassnameVariable(String *tm, const char *classnamevariable, SwigType *type);
    Parm *skipIgnoredArgs(Parm *p);
    void marshalInputArgs(ParmList *parmlist, Wrapper *wrapper);
    void makeParameterList(ParmList *parmlist, Wrapper *wrapper);
    String *makeParameterName(Node *n, Parm *p, int arg_num, bool setter) const;
    void marshalOutput(Node *n, String *actioncode, Wrapper *wrapper);
    String *createProxyName(SwigType *t);
    const String *typemapLookup(Node *n, const_String_or_char_ptr tmap_method, SwigType *type, int warning, Node *typemap_attributes = 0);
    
    virtual void replaceSpecialVariables(String *method, String *tm, Parm *parm);
    virtual int doxygenComment(Node *n);
};

/* -----------------------------------------------------------------------------
* directorClassName()
* ----------------------------------------------------------------------------- */

String *OBJECTIVEC::directorClassName(Node *n) {
    String *dirclassname;
    const char *attrib = "director:classname";

    if (!(dirclassname = Getattr(n, attrib))) {
        String *classname = Getattr(n, "sym:name");

        dirclassname = NewStringf("SwigDirector_%s", classname);
        Setattr(n, attrib, dirclassname);
    }

    return dirclassname;
}

/* ---------------------------------------------------------------------
 * main()
 *
 * Entry point for the Objective-C module.
 * --------------------------------------------------------------------- */

void OBJECTIVEC::main(int argc, char *argv[]) {
    // Set language-specific subdirectory in SWIG library
    SWIG_library_directory("objc");
    
    bool debug_doxygen_translator = false;
    bool debug_doxygen_parser = false;
    
    // Process command line options
    for (int i = 1; i < argc; i++) {
        if (argv[i]) {
            if (strcmp(argv[i], "-doxygen") == 0) {
                Swig_mark_arg(i);
                doxygen = true;
                scan_doxygen_comments = true;
            } else if (strcmp(argv[i], "-debug-doxygen-translator") == 0) {
                Swig_mark_arg(i);
                debug_doxygen_translator = true;
            } else if (strcmp(argv[i], "-debug-doxygen-parser") == 0) {
                Swig_mark_arg(i);
                debug_doxygen_parser = true;
            } else if (strcmp(argv[i], "-noproxy") == 0) {
                Swig_mark_arg(i);
                proxy_flag = false;
            } else if (strcmp(argv[i], "-help") == 0) {
                Printf(stdout, "%s\n", usage);
            }
        }
    }
    
    // Use java doc translator for now
    if (doxygen)
        doxygenTranslator = new JavaDocConverter("objc", debug_doxygen_translator, debug_doxygen_parser);
    
    // Set language-specific preprocessing symbol
    Preprocessor_define("SWIGOBJECTIVEC 1", 0);
    
    // Set language-specific configuration file
    SWIG_config_file("objc.swg");

    SWIG_config_cppext("mm");
    
    // Set typemap language (historical)
    SWIG_typemap_lang("objc");

    // ObjC allows overloading
    allow_overloading();
}

/* ---------------------------------------------------------------------
 * top()
 *
 * Function handler for processing top node of the parse tree
 * Wrapper code generation starts from here.
 * --------------------------------------------------------------------- */

int OBJECTIVEC::top(Node *n) {
    /* Get the module name */
    String *module = Getattr(n, "name");
    
    // Get any options set in the module directive
    Node *optionsnode = Getattr(Getattr(n, "module"), "options");

    if (optionsnode) {
        /* check if directors are enabled for this module.  note: this
        * is a "master" switch, without which no director code will be
        * emitted.  %feature("director") statements are also required
        * to enable directors for individual classes or methods.
        *
        * use %module(directors="1") modulename at the start of the
        * interface file to enable director generation.
        */
        if (Getattr(optionsnode, "directors")) {
            allow_directors();
        }
        if (Getattr(optionsnode, "dirprot")) {
            allow_dirprot();
        }
        allow_allprotected(GetFlag(optionsnode, "allprotected"));
    }

    /* Initialize I/O */
    String *outfile = Getattr(n, "outfile");
    String *outfile_h = Getattr(n, "outfile_h");

    if (!outfile) {
        Printf(stderr, "Unable to determine outfile\n");
        SWIG_exit(EXIT_FAILURE);
    }

    // Create the _wrap files
    String *wrapfile_h = NewString(outfile_h);
    f_wrap_h = NewFile(wrapfile_h, "w", SWIG_output_files());
    if (!f_wrap_h) {
        FileErrorDisplay(wrapfile_h);
        SWIG_exit(EXIT_FAILURE);
    }
    String *wrapfile_mm = NewString(outfile);
    f_wrap_mm = NewFile(wrapfile_mm, "w", SWIG_output_files());
    if (!f_wrap_mm) {
        FileErrorDisplay(wrapfile_mm);
        SWIG_exit(EXIT_FAILURE);
    }
    Delete(wrapfile_h);
    Delete(wrapfile_mm);
    
    // Create the _proxy files if proxy flag is true
    if (proxy_flag) {
        String *proxyfile_h = NewStringf("%s%s_proxy.h", SWIG_output_directory(), module);
        f_proxy_h = NewFile(proxyfile_h, "w", SWIG_output_files());
        if (!f_proxy_h) {
            FileErrorDisplay(proxyfile_h);
            SWIG_exit(EXIT_FAILURE);
        }
        String *proxyfile_mm = NewStringf("%s%s_proxy.mm", SWIG_output_directory(), module);
        f_proxy_mm = NewFile(proxyfile_mm, "w", SWIG_output_files());
        if (!f_proxy_mm) {
            FileErrorDisplay(proxyfile_mm);
            SWIG_exit(EXIT_FAILURE);
        }
        Delete(proxyfile_h);
        Delete(proxyfile_mm);
    }
    
    f_runtime = NewString("");
    f_init = NewString("");
    f_header = NewString("");
    f_wrappers = NewString("");
    f_directors_h = NewString("");
    f_directors = NewString("");

    
    // Register file targets with the SWIG file handler
    Swig_register_filebyname("begin", f_wrap_mm);
    Swig_register_filebyname("header", f_header);
    Swig_register_filebyname("wrapper", f_wrappers);
    Swig_register_filebyname("runtime", f_runtime);
    Swig_register_filebyname("init", f_init);
    Swig_register_filebyname("director", f_directors);
    Swig_register_filebyname("director_h", f_directors_h);

    // Output module initialization code
    Swig_banner(f_wrap_h);
    Printf(f_wrap_h, "\n");
    Printf(f_wrap_h, "#ifndef %(upper)s_WRAP_H\n", module);
    Printf(f_wrap_h, "#define %(upper)s_WRAP_H\n\n", module);
    Printf(f_wrap_h, "\n#ifdef __cplusplus\n");
    Printf(f_wrap_h, "extern \"C\" {\n");
    Printf(f_wrap_h, "#endif\n\n");
    
    Swig_banner(f_wrap_mm);
    
    if (proxy_flag) {
        String* wrapper_name = NewStringf("Objc_%s_%%f", module);
        Swig_name_register("wrapper", Char(wrapper_name));
        
        Swig_banner(f_proxy_h);
        Printf(f_proxy_h, "\n#import <Foundation/Foundation.h>\n\n");
        
        // ObjectiveC will understand the C code.
        Printf(f_proxy_h, "\n#ifdef __cplusplus\n");
        Printf(f_proxy_h, "extern \"C\" {\n");
        Printf(f_proxy_h, "#endif\n\n");
        
        Swig_banner(f_proxy_mm);
        Printf(f_proxy_mm, "#include \"%s_proxy.h\"\n", module);
        Printf(f_proxy_mm, "#include \"%s_wrap.mm\"\n\n", module);
    }
    // Create strings for holding the generated code. These will be dumped
    // to the generated files at the end of the top function.
    //wrap_h_code = NewString("");
    wrap_mm_code = NewString("");
    if (proxy_flag) {
        proxy_h_code = NewString("");
        proxy_mm_code = NewString("");
        swigtypes_h_code = NewString("");
        swigtypes_mm_code = NewString("");
        
        proxy_class_decl_code = NewString("");
        proxy_class_defn_code = NewString("");
        proxy_class_enums_code = NewString("");
        proxy_class_function_decls = NewString("");
        proxy_class_function_defns = NewString("");
        proxy_global_function_decls = NewString("");
        proxy_global_function_defns = NewString("");
        proxy_class_decl_imports = NewString("");
        proxy_class_defn_imports = NewString("");
        
        destrcutor_call = NewString("");
        structuralComments = NewString("");
        unknown_types = NewHash();
    }

    if (directorsEnabled()) {
        Printf(f_runtime, "#define SWIG_DIRECTORS\n");

        Printf(f_directors, "\n\n");
        Printf(f_directors, "/* ---------------------------------------------------\n");
        Printf(f_directors, " * C++ director class methods\n");
        Printf(f_directors, " * --------------------------------------------------- */\n\n");
        Printf(f_directors, "#include <objc/runtime.h>\n");
        Printf(f_directors, "#include \"%s_wrap.h\"\n\n", module);
    }

    
    /* Emit code for children */
    Language::top(n);
    
    if (directorsEnabled()) {
        // Insert director runtime into the f_runtime file (make it occur before %header section)
        Swig_insert_file("director.swg", f_runtime);
    }

    // Write to the wrap_mm
    Dump(f_runtime, f_wrap_mm);
    Dump(f_header, f_wrap_mm);

    // Copy director code
    if (directorsEnabled()) {
        Dump(f_directors, f_wrap_mm);
        Dump(f_directors_h, f_wrap_h);

        Delete(f_directors);
        f_directors = NULL;
        Delete(f_directors_h);
        f_directors_h = NULL;
    }

    Printf(f_wrap_mm, "\n#ifdef __cplusplus\n");
    Printf(f_wrap_mm, "extern \"C\" {\n");
    Printf(f_wrap_mm, "#endif\n\n");
    Dump(f_wrappers, f_wrap_mm);
    Dump(wrap_mm_code, f_wrap_mm);
    Wrapper_pretty_print(f_init, f_wrap_mm);
    Printf(f_wrap_mm, "\n#ifdef __cplusplus\n");
    Printf(f_wrap_mm, "}\n");
    Printf(f_wrap_mm, "#endif\n");

    // Write to the wrap_h
    Printf(f_wrap_h, "\n#ifdef __cplusplus\n");
    Printf(f_wrap_h, "}\n");
    Printf(f_wrap_h, "#endif\n");
    Printf(f_wrap_h, "\n#endif\n\n");

    // Write to the proxy.h, if required
    if (proxy_flag) {
        for (Iterator swig_type = First(unknown_types); swig_type.key; swig_type = Next(swig_type)) {
            emitTypeWrapperClass(swig_type.key, swig_type.item);
        }
        
        Dump(swigtypes_h_code, f_proxy_h);
        Dump(proxy_h_code, f_proxy_h);
        Printf(f_proxy_h, "\n#ifdef __cplusplus\n");
        Printf(f_proxy_h, "}\n");
        Printf(f_proxy_h, "#endif\n\n");
    }
    // Write to proxy.mm, if required
    if (proxy_flag) {
        //        Printf(f_proxy_mm, "\n#ifdef __cplusplus\n");
        //        Printf(f_proxy_mm, "extern \"C\" {\n");
        //        Printf(f_proxy_mm, "#endif\n\n");
        Dump(swigtypes_mm_code, f_proxy_mm);
        Dump(proxy_mm_code, f_proxy_mm);
        //        Printf(f_proxy_mm, "\n#ifdef __cplusplus\n");
        //        Printf(f_proxy_mm, "}\n");
        //        Printf(f_proxy_mm, "#endif\n");
    }
    // Cleanup
    Delete(wrap_mm_code);
    Delete(f_runtime);
    Delete(f_init);
    Delete(f_header);
    Delete(f_wrappers);
    Delete(f_wrap_mm);
    
    if (proxy_flag) {
        Delete(proxy_class_decl_code);
        Delete(proxy_class_defn_code);
        Delete(proxy_class_enums_code);
        Delete(proxy_class_function_decls);
        Delete(proxy_class_function_defns);
        Delete(proxy_global_function_decls);
        Delete(proxy_global_function_defns);
        Delete(proxy_class_decl_imports);
        Delete(proxy_class_defn_imports);
        Delete(destrcutor_call);
        Delete(structuralComments);
        Delete(unknown_types);
        
        Delete(swigtypes_h_code);
        Delete(swigtypes_mm_code);
        Delete(proxy_h_code);
        Delete(proxy_mm_code);
        Delete(f_proxy_h);
        Delete(f_proxy_mm);
    }
    
    return SWIG_OK;
}

/* -----------------------------------------------------------------------------
* getOverloadedName()
* ----------------------------------------------------------------------------- */

String *OBJECTIVEC::getOverloadedName(Node *n) {

    /* The intermediary class methods need to be mangled when overloaded to give
     * a unique name. */
    String *overloaded_name = NewStringf("%s", Getattr(n, "sym:name"));

    if (Getattr(n, "sym:overloaded")) {
        Printv(overloaded_name, Getattr(n, "sym:overname"), NIL);
    }

    return overloaded_name;
}

/* ---------------------------------------------------------------------
 * functionHandler()
 *
 * Function handler for generating wrappers for functions
 * --------------------------------------------------------------------- */

int OBJECTIVEC::functionHandler(Node *n) {
    Language::functionHandler(n);
    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * globalfunctionHandler()
 *
 * Function handler for generating wrappers for functions
 * --------------------------------------------------------------------- */

int OBJECTIVEC::globalfunctionHandler(Node *n) {
    global_func_flag = true;
    Language::globalfunctionHandler(n);
    global_func_flag = false;
    
    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * memberfunctionHandler()
 *
 * Function handler for generating wrappers for functions
 * --------------------------------------------------------------------- */

int OBJECTIVEC::memberfunctionHandler(Node *n) {
    String *symname = Getattr(n, "sym:name");
    proxyfuncname = symname;
    member_func_flag = true;
    Language::memberfunctionHandler(n);
    member_func_flag = false;
    
    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * staticmemberfunctionHandler()
 *
 * Function handler for generating wrappers for functions
 * --------------------------------------------------------------------- */

int OBJECTIVEC::staticmemberfunctionHandler(Node *n) {
    String *symname = Getattr(n, "sym:name");
    proxyfuncname = symname;
    static_member_func_flag = true;
    Language::staticmemberfunctionHandler(n);
    static_member_func_flag = false;
    
    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * callbackfunctionHandler()
 *
 * Function handler for generating wrappers for functions
 * --------------------------------------------------------------------- */

int OBJECTIVEC::callbackfunctionHandler(Node *n) {
    Language::callbackfunctionHandler(n);
    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * variableHandler()
 *
 * Function handler for generating wrappers for functions
 * --------------------------------------------------------------------- */

int OBJECTIVEC::variableHandler(Node *n) {
    Language::variableHandler(n);
    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * globalvariableHandler()
 *
 * Function handler for generating wrappers for functions
 * --------------------------------------------------------------------- */

int OBJECTIVEC::globalvariableHandler(Node *n) {
    String *symname = Getattr(n, "sym:name");
    variable_name = symname;
    global_variable_flag = true;
    
    Language::globalvariableHandler(n);
    
    global_variable_flag = false;
    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * membervariableHandler()
 *
 * Function handler for generating wrappers for functions
 * --------------------------------------------------------------------- */

int OBJECTIVEC::membervariableHandler(Node *n) {
    String *symname = Getattr(n, "sym:name");
    variable_name = symname;
    member_variable_flag = true;
    
    Language::membervariableHandler(n);
    
    member_variable_flag = false;
    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * staticmembervariableHandler()
 *
 * Function handler for generating wrappers for functions
 * --------------------------------------------------------------------- */

int OBJECTIVEC::staticmembervariableHandler(Node *n) {
    String *symname = Getattr(n, "sym:name");
    variable_name = symname;
    static_member_variable_flag = true;
    
    Language::staticmembervariableHandler(n);
    
    static_member_variable_flag = false;
    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * memberconstantHandler()
 *
 * Function handler for generating wrappers for functions
 * --------------------------------------------------------------------- */

int OBJECTIVEC::memberconstantHandler(Node *n) {
    String *symname = Getattr(n, "sym:name");
    variable_name = symname;
    member_constant_flag = true;
    
    Language::memberconstantHandler(n);
    
    member_constant_flag = false;
    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * constructorHandler()
 *
 * Function handler for generating wrappers for functions
 * --------------------------------------------------------------------- */

int OBJECTIVEC::constructorHandler(Node *n) {
    Language::constructorHandler(n);
    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * copyconstructorHandler()
 *
 * Function handler for generating wrappers for functions
 * --------------------------------------------------------------------- */

int OBJECTIVEC::copyconstructorHandler(Node *n) {
    Language::copyconstructorHandler(n);
    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * destructorHandler()
 *
 * Function handler for generating wrappers for functions
 * --------------------------------------------------------------------- */

int OBJECTIVEC::destructorHandler(Node *n) {
    Language::destructorHandler(n);
    String *symname = Getattr(n, "sym:name");
    Printv(destrcutor_call, Swig_name_wrapper(Swig_name_destroy(getNSpace(), symname)), "((void*)swigCPtr)", NIL);
    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * classHandler()
 *
 * Function handler for generating wrappers for functions
 * --------------------------------------------------------------------- */

int OBJECTIVEC::classHandler(Node *n) {
    if (proxy_flag) {
        proxy_class_name = Copy(Getattr(n, "sym:name"));
        
        if (!addSymbol(proxy_class_name, n))
            return SWIG_ERROR;
        
        //Clear(proxy_class_imports);
        Clear(proxy_class_function_decls);
        Clear(proxy_class_function_defns);
        Clear(proxy_global_function_decls);
        Clear(proxy_global_function_defns);
        Clear(proxy_class_enums_code);
        Clear(proxy_class_decl_code);
        Clear(proxy_class_defn_code);
        Clear(destrcutor_call);
    }
    
    Language::classHandler(n);
    
    if (proxy_flag) {
        // Write the code for proxy class
        emitProxyClass(n);
        
        // Apply the necessary substitutions
        Replaceall(proxy_class_decl_code, "$objcclassname", proxy_class_name);
        Replaceall(proxy_class_defn_code, "$objcclassname", proxy_class_name);
        
        // And, dump everything to the proxy files
        Printv(proxy_h_code, proxy_class_decl_code, NIL);
        Printv(proxy_mm_code, proxy_class_defn_code, NIL);
        Printv(proxy_h_code,proxy_global_function_decls,NIL);
        Printv(proxy_mm_code,proxy_global_function_defns,NIL);
        // Tidy up
        Delete(proxy_class_qname);
        proxy_class_qname = NULL;
        Delete(proxy_class_name);
        proxy_class_name = NULL;
    }

    emitDirectorExtraMethods(n);
    
    return SWIG_OK;
}

/* ----------------------------------------------------------------------
 * enumDeclaration()
 *
 * Wraps C/C++ enums as Objective C enums
 * ---------------------------------------------------------------------- */

int OBJECTIVEC::enumDeclaration(Node *n) {
    if (ImportMode)
        return SWIG_OK;
    
    if (getCurrentClass() && (cplus_mode != PUBLIC))
        return SWIG_NOWRAP;
    
    String *typemap_lookup_type = Getattr(n, "name");
    String *symname = Getattr(n, "sym:name");
    String *enumname;
    
    Node *p = parentNode(n);
    if (p && !Strcmp(nodeType(p), "class")) {	// This is a nested enum, prefix the class name
        String *parentname = Getattr(p, "sym:name");
        enumname = NewStringf("%s_%s", parentname, symname);
    } else {
        enumname = Copy(symname);
    }
    
    if (proxy_flag) {
        // Write documentation
        if (doxygen && doxygenTranslator->hasDocumentation(n)){
            String *doxygen_comments = doxygenTranslator->getDocumentation(n);
            if(comment_creation_chatter)
                Printf(proxy_h_code, "/* This was generated from enumDeclaration() */");
            Printv(proxy_h_code, Char(doxygen_comments), NIL);
            Delete(doxygen_comments);
        }
        
        if (typemap_lookup_type) {
            // Copy-paste the C/C++ enum as an Objective-C enum
            Printv(proxy_h_code, "typedef NS_ENUM(NSInteger, ", enumname, ") {\n", NIL);
        } else {
            // Handle anonymous enums.
            Printv(proxy_h_code, "\nenum {\n", NIL);
        }
    }
    // Emit each enum item
    Language::enumDeclaration(n);
    
    if (!GetFlag(n, "nonempty")) {
        // Do not wrap empty enums;
        return SWIG_NOWRAP;
    }
    
    if (proxy_flag) {
        if (typemap_lookup_type) {
            // Finish the enum declaration
            Printv(proxy_h_code, "\n};\n\n", NIL);
        } else {
            // Handle anonymous enums.
            Printv(proxy_h_code, "\n};\n\n", NIL);
        }
    }
    
    Delete(enumname);
    return SWIG_OK;
}

/* ---------------------------------------------------------------------------
 * enumvalueDeclaration()
 *
 * Genrerates the enum item for the Objective C enums
 * --------------------------------------------------------------------------- */
int OBJECTIVEC::enumvalueDeclaration(Node *n) {
    if (getCurrentClass() && (cplus_mode != PUBLIC))
        return SWIG_NOWRAP;
    
    Swig_require("enumvalueDeclaration", n, "*name", "?value", NIL);
    String *symname = Getattr(n, "sym:name");
    String *value = Getattr(n, "enumvalue");
    Node *parent = parentNode(n);
    Node *pparent = parentNode(parent);
    String *enumname;
    if (pparent && !Strcmp(nodeType(pparent), "class")) {	// This is a nested enum, prefix the class name
        String *classname = Getattr(pparent, "sym:name");
        enumname = NewStringf("%s_%s", classname, symname);
    } else {
        enumname = Copy(symname);
    }
    if (proxy_flag) { // Emit the enum item
        if (!GetFlag(n, "firstenumitem"))
            Printf(proxy_h_code, ",\n");
        
        // Write documentation
        if (doxygen && doxygenTranslator->hasDocumentation(n)){
            String *doxygen_comments = doxygenTranslator->getDocumentation(n);
            if(comment_creation_chatter)
                Printf(proxy_h_code, "/* This was generated from enumvalueDeclaration() */");
            Printv(proxy_h_code, Char(doxygen_comments), NIL);
            Delete(doxygen_comments);
        }
        
        Printf(proxy_h_code, "  %s", enumname);
        if (value) {
            value = Getattr(n, "enumvalue") ? Copy(Getattr(n, "enumvalue")) : Copy(Getattr(n, "enumvalueex"));
            Printf(proxy_h_code, " = %s", value);
        }
    }
    // Keep track that the currently processed enum has at least one value
    SetFlag(parent, "nonempty");
    
    Swig_restore(n);
    Delete(enumname);
    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * constantWrapper()
 *
 * Low level code generator for constants
 * --------------------------------------------------------------------- */

int OBJECTIVEC::constantWrapper(Node *n) {
    SwigType *type = Getattr(n, "type");
    String *symname = Getattr(n, "sym:name");
    String *value = Getattr(n, "value");
    String *typestring = SwigType_str(type, 0);
    String *tm;
    
    String *crettype = NewString("");
    String *constants_h_code = NewString("");
    String *constants_mm_code = NewString("");
    int is_func_ptr = SwigType_isfunctionpointer(type);
    
    if (!addSymbol(symname, n))
        return SWIG_ERROR;
    
    // Get the corresponding ObjectiveC type or the intermediate type. "imtype" if no proxy and "objctype" if proxy_flag is true.
    if (!is_func_ptr) {
        if (proxy_flag) {
            if ((tm = Swig_typemap_lookup("objctype", n, "", 0))) {
                substituteClassname(tm, type);
                Printf(crettype, "%s", tm);
            } else {
                Swig_warning(WARN_OBJC_TYPEMAP_OBJCTYPE_UNDEF, input_file, line_number, "No objctype typemap defined for %s\n", typestring);
            }
        } else {
            if ((tm = Swig_typemap_lookup("imtype", n, "", 0))) {
                Printf(crettype, "%s", tm);
            } else {
                Swig_warning(WARN_OBJC_TYPEMAP_IMTYPE_UNDEF, input_file, line_number, "No imtype typemap defined for %s\n", typestring);
            }
        }
    } else {
        if (proxy_flag) {
            if ((tm = Swig_typemap_lookup("objctype", n, "", 0))) {
                substituteClassname(tm, type);
                Printf(crettype, "%s", tm);
            } else {
                Swig_warning(WARN_OBJC_TYPEMAP_OBJCTYPE_UNDEF, input_file, line_number, "No objctype typemap defined for %s\n", typestring);
            }
        } else {
            Printf(crettype, "%s", SwigType_str(type, symname));
        }
    }
    
    if (is_func_ptr) {
        variableWrapper(n);
        Printf(constants_h_code, "extern %s %s;\n", crettype, symname);
        //Printf(constants_mm_code, "%s %s = (%s)%s();\n", crettype, symname, typestring, Swig_name_wrapper(Swig_name_get(getNSpace(), symname)));
        // Transform return type used in low level accessor to the type used in Objective-C constant definition
        String *imcall = NewStringf("%s()", Swig_name_wrapper(Swig_name_get(getNSpace(), symname)));
        if ((tm = Swig_typemap_lookup("objcvarout", n, "", 0))) {
            substituteClassname(tm, type);
            Replaceall(tm, "$objcvarname", symname);
            Replaceall(tm, "$imcall", imcall);
        } else {
            Swig_warning(WARN_OBJC_TYPEMAP_OBJCOUT_UNDEF, input_file, line_number, "No objcout typemap defined for %s\n", crettype);
        }
        Printf(constants_mm_code, "%s\n", tm);
        Delete(imcall);
    }else if (Getattr(n, "wrappedasconstant")) {
        if (SwigType_type(type) == T_CHAR){
            Printf(constants_mm_code, "%s %s= \'%s\';\n", crettype, symname, Getattr(n, "staticmembervariableHandler:value"));
        }
        else if (SwigType_type(type) == T_STRING){
            Printf(constants_mm_code, "%s const %s= @\"%s\";\n", crettype, symname, Getattr(n, "staticmembervariableHandler:value"));
        } else {
            Printf(constants_mm_code, "%s %s= %s;\n", crettype, symname, Getattr(n, "staticmembervariableHandler:value"));
        }
    } else {
        if (SwigType_type(type) == T_STRING) {
            // http://stackoverflow.com/questions/538996/constants-in-objective-c/539191#539191
            Printf(constants_h_code, "extern %s const %s;\n", crettype, symname);
            Printf(constants_mm_code, "%s const %s= @\"%s\";\n", crettype, symname, value);
        } else if (SwigType_type(type) == T_CHAR) {
            Printf(constants_h_code, "extern %s %s;\n", crettype, symname);
            Printf(constants_mm_code, "%s %s= \'%s\';\n", crettype, symname, value);
        } else {
            Printf(constants_h_code, "extern %s %s;\n", crettype, symname);
            Printf(constants_mm_code, "%s %s= %s;\n", crettype, symname, value);
        }
    }
    
    // Dump to generated files
    if (proxy_flag) {		// write to the proxy files
        // Write documentation
        if (doxygen && doxygenTranslator->hasDocumentation(n)){
            String *doxygen_comments = doxygenTranslator->getDocumentation(n);
            if(comment_creation_chatter)
                Printf(proxy_h_code, "/* This was generated from constantWrapper() */");
            Printv(proxy_h_code, Char(doxygen_comments), NIL);
            Delete(doxygen_comments);
        }
        
        Printv(proxy_h_code, constants_h_code, NIL);
        Printv(proxy_mm_code, constants_mm_code, NIL);
    } else {			// write to the wrap files
        //Printv(wrap_h_code, constants_h_code, NIL);
        Printv(wrap_mm_code, constants_mm_code, NIL);
    }
    
    // Cleanup
    Delete(crettype);
    Delete(constants_h_code);
    Delete(constants_mm_code);
    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * variableWrapper()
 *
 * Low level code generator for variables
 * --------------------------------------------------------------------- */

int OBJECTIVEC::variableWrapper(Node *n) {
    Language::variableWrapper(n);
    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * functionWrapper()
 *
 *
 * Generates the C wrapper code for a function and the corresponding
 * declaration in the wrap files
 * --------------------------------------------------------------------- */

int OBJECTIVEC::functionWrapper(Node *n) {
    // Get some useful attributes of this function
    String *symname = Getattr(n, "sym:name");
    SwigType *type = Getattr(n, "type");
    ParmList *parmlist = Getattr(n, "parms");
    String *crettype = SwigType_str(type, 0);
    String *imrettype = NewString("");
    String *tm;
    String *overloaded_name = getOverloadedName(n);
    bool is_void_return = (Cmp(crettype, "void") == 0);
    bool is_constructor = (Cmp(nodeType(n), "constructor") == 0);
    bool is_destructor = (Cmp(nodeType(n), "destructor") == 0);
    bool is_constant = (Cmp(nodeType(n), "constant") == 0);
    
    if (!Getattr(n, "sym:overloaded")) {
        if (!addSymbol(Getattr(n, "sym:name"), n))
            return SWIG_ERROR;
    }
    // Create the function's wrappered name
    String *wname = Swig_name_wrapper(overloaded_name);
    
    // Create the wrapper function object
    Wrapper *wrapper = NewWrapper();
    
    // Retrieve the intermediate return type for this function
    if ((tm = Swig_typemap_lookup("imtype", n, "", 0))) {
        Printf(imrettype, "%s", tm);
    } else {
        Swig_warning(WARN_OBJC_TYPEMAP_IMTYPE_UNDEF, input_file, line_number, "No imtype typemap defined for %s\n", crettype);
    }
    
    // Deal with overloading
    
    /* Write the wrapper function declaration and definition */
    
    // Begin the first line of the function definition
    Printv(wrapper->def, imrettype, " ", wname, "(", NIL);
    
    // Make the param list with the intermediate parameter types
    makeParameterList(parmlist, wrapper);
    
    // End the first line of the function definition
    Printv(wrapper->def, ")", NIL);
    
    // Write the function declaration to the wrap_h
    //Printv(wrap_h_code, wrapper->def, ";\n", NIL);
    
    // Now write the function definition to the wrap_cpp
    Printv(wrapper->def, "\n{", NIL);
    
    // Emit all of the local variables for holding arguments
    emit_parameter_variables(parmlist, wrapper);
    
    // If any additional local variable needed, add them now
    if (!is_void_return)
        Wrapper_add_localv(wrapper, "imresult", imrettype, "imresult = 0", NIL);
    
    // Attach the standard typemaps to the parameter list
    emit_attach_parmmaps(parmlist, wrapper);
    Setattr(n, "wrap:parms", parmlist);
    
    // Now walk the function parameter list and generate code to get arguments.
    marshalInputArgs(parmlist, wrapper);
    
    // Emit the function call
    if (is_constant) {		// Wrapping a constant hack.
        Swig_save("functionWrapper", n, "wrap:action", NIL);
        
        // below based on Swig_VargetToFunction()
        SwigType *ty = Swig_wrapped_var_type(Getattr(n, "type"), use_naturalvar_mode(n));
        Setattr(n, "wrap:action", NewStringf("%s = (%s) %s;", Swig_cresult_name(), SwigType_lstr(ty, 0), Getattr(n, "value")));
    }
    
    Swig_director_emit_dynamic_cast(n, wrapper);
    
    Setattr(n, "wrap:name", wname);
    String *actioncode = emit_action(n);

    if (Cmp(nodeType(n), "constant") == 0)
        Swig_restore(n);
    
    // Write typemaps(out) and return value if necessary.
    marshalOutput(n, actioncode, wrapper);
    
    // Close the function
    if (!is_void_return)
        Printv(wrapper->code, "return imresult;\n", NIL);
    Printv(wrapper->code, "}", NIL);
    
    // Final substititions if applicable.
    
    /* Contract macro modification */
    Replaceall(wrapper->code, "SWIG_contract_assert(", "SWIG_contract_assert($null, ");
    
    if (!is_void_return)
        Replaceall(wrapper->code, "$null", "0");
    else
        Replaceall(wrapper->code, "$null", "");
    
    // Dump the function out.
    Wrapper_print(wrapper, wrap_mm_code);
    
    /* Create the proxy functions if proxy_flag is true. */
    if (proxy_flag && is_constructor) {	// Handle constructor
        Setattr(n, "imfunctionname", wname);
        emitProxyClassConstructor(n);
    } else if (proxy_flag && is_destructor) {	// Handle destructor
        // TODO: Do it here instead in emitProxyClass function
    }
    // globalFunctionHandler is called for static member functions as well hence setting global_func_flag to true.
    // To route the call to the appropriate proxy generator, we check for !static_member_func_flag here.
    if (proxy_flag && (global_variable_flag || global_func_flag) && !static_member_func_flag) {	// Handle globals
        Setattr(n, "imfunctionname", wname);
        emitProxyGlobalFunctions(n);
    } else if (proxy_flag && (member_variable_flag || static_member_variable_flag || member_constant_flag || member_func_flag || static_member_func_flag)) {	// Handle members
        Setattr(n, "imfunctionname", wname);
        emitProxyClassFunction(n);
    }
    // Tidy up
    Delete(overloaded_name);
    Delete(imrettype);
    Delete(wname);
    DelWrapper(wrapper);
    return SWIG_OK;
}

/* ---------------------------------------------------------------------
 * nativeWrapper()
 *
 *
 * --------------------------------------------------------------------- */

int OBJECTIVEC::nativeWrapper(Node *n) {
    Language::nativeWrapper(n);
    return SWIG_OK;
}

/* -----------------------------------------------------------------------------
 * emitProxyGlobalFunctions()
 *
 * C/C++ global functions and global variables are mapped to ObjectiveC global functions.
 * C/C++ types will be replaced with their equivalent ObjectiveC types (eg: char* --> NSString*)
 * A prefix 'Objc' will be added to counter linker redifiniton complains.
 *
 * ----------------------------------------------------------------------------- */

void OBJECTIVEC::emitProxyGlobalFunctions(Node *n) {
    if (!proxy_flag)
        return;
    
    // Get some useful attributes of this function
    String *symname = Getattr(n, "sym:name");
    SwigType *type = Getattr(n, "type");
    ParmList *parmlist = Getattr(n, "parms");
    String *crettype = SwigType_str(type, 0);
    String *storage = Getattr(n, "storage");
    String *objcrettype = NewString("");
    String *imcall = NewString("");
    String *function_defn = NewString("");
    String *function_decl = NewString("");
    String *tm;
    String *imfunctionname = Getattr(n, "imfunctionname");
    String *proxyfunctionname;
    bool setter_flag = false;
    
    // Retrieve the ObjectiveC return type for this function
    if ((tm = Swig_typemap_lookup("objctype", n, "", 0))) {
        substituteClassname(tm, type);
        Printf(objcrettype, "%s", tm);
    } else {
        Swig_warning(WARN_OBJC_TYPEMAP_OBJCTYPE_UNDEF, input_file, line_number, "No objctype typemap defined for %s\n", crettype);
    }
    
    // Deal with overloading
    
    // Change the function name for global variables
    if (proxy_flag && global_variable_flag) {
        proxyfunctionname = NewString("");
        setter_flag = (Cmp(symname, Swig_name_set(getNSpace(), variable_name)) == 0);
        if (setter_flag)
            Printf(proxyfunctionname, "set");
        else
            Printf(proxyfunctionname, "get");
        
        // Capitalize the first letter in the variable to create an ObjectiveC proxy function name
        Putc(toupper((int) *Char(variable_name)), proxyfunctionname);
        Printf(proxyfunctionname, "%s", Char(variable_name) + 1);
    } else {
        proxyfunctionname = NewString("Objc");
        // Capitalize the first letter
        Putc(toupper((int) *Char(symname)), proxyfunctionname);
        Printf(proxyfunctionname, "%s", Char(symname) + 1);
    }
    
    /* Write the proxy global function declaration and definition */
    
    // Begin the first line of the function
    Printv(function_decl, objcrettype, " ", proxyfunctionname, "(", NIL);
    
    // Prepare the call to intermediate function
    Printf(imcall, "%s(", imfunctionname);
    
    // Attach the non-standard typemaps to the parameter list
    Swig_typemap_attach_parms("in", parmlist, NULL);
    Swig_typemap_attach_parms("objctype", parmlist, NULL);
    Swig_typemap_attach_parms("objcin", parmlist, NULL);
    
    Parm *p;
    int i = 0;
    int gencomma = 0;
    
    for (p = parmlist; p; p = nextSibling(p), i++) {
        p = skipIgnoredArgs(p);
        SwigType *pt = Getattr(p, "type");
        
        String *objcparmtype = NewString("");
        
        // Get the ObjectiveC parameter type for this parameter
        if ((tm = Getattr(p, "tmap:objctype"))) {
            substituteClassname(tm, pt);
            Printf(objcparmtype, "%s", tm);
        } else {
            Swig_warning(WARN_OBJC_TYPEMAP_OBJCTYPE_UNDEF, input_file, line_number, "No objctype typemap defined for %s\n", SwigType_str(pt, 0));
        }
        
        if (gencomma)
            Printf(imcall, ", ");
        
        String *arg = makeParameterName(n, p, i, setter_flag);
        
        // Use typemaps to transform the type used in Objective-C proxy function to the one used in intermediate code.
        if ((tm = Getattr(p, "tmap:objcin"))) {
            substituteClassname(tm, pt);
            Replaceall(tm, "$objcinput", arg);
            Printv(imcall, tm, NIL);
        } else {
            Swig_warning(WARN_OBJC_TYPEMAP_OBJCIN_UNDEF, input_file, line_number, "No objcin typemap defined for %s\n", SwigType_str(pt, 0));
        }
        
        // Add parameter to proxy function
        if (gencomma >= 2)
            Printf(function_decl, ", ");
        gencomma = 2;
        Printf(function_decl, "%s %s", objcparmtype, arg);
        
        Delete(arg);
        Delete(objcparmtype);
    }
    
    // End the first line of the function
    Printv(function_decl, ")", NIL);	// or
    //Printv(function_decl, paramstring, ")", NIL);
    
    Printv(function_defn, function_decl, "\n{\n", NIL);
    Printf(function_decl, ";");
    
    // End the call to the intermediate function
    Printv(imcall, ")", NIL);
    
    // Transform return type used in low level accessor to type used in Objective-C proxy function
    if ((tm = Swig_typemap_lookup("objcout", n, "", 0))) {
        substituteClassname(tm, type);
        Replaceall(tm, "$imcall", imcall);
    } else {
        Swig_warning(WARN_OBJC_TYPEMAP_OBJCOUT_UNDEF, input_file, line_number, "No objcout typemap defined for %s\n", crettype);
    }
    
    Printf(function_defn, " %s\n}\n", tm ? ((const String *) tm) : empty_string);
    
    // Retrieve docymentation
    String* documentation = NewString("");
    if (doxygen && doxygenTranslator->hasDocumentation(n)){
        String *doxygen_comments = doxygenTranslator->getDocumentation(n);
        if(comment_creation_chatter)
            Printf(documentation, "/* This was generated from emitProxyGlobalFunctions() */");
        Printv(documentation, Char(doxygen_comments), NIL);
        Delete(doxygen_comments);
    }
    
    /* Write the function declaration to the proxy_h_code
     and function definition to the proxy_mm_code */
    if ((member_func_flag || member_constant_flag || Strcmp(storage, "friend") == 0 || Strcmp(storage, "typedef") == 0))
    {
        // Write documentation
        Printf(proxy_global_function_decls, "%s", documentation);
        
        Printv(proxy_global_function_decls, function_decl, "\n", NIL);
        Printv(proxy_global_function_defns, function_defn, "\n", NIL);
    }else {
        // Write documentation
        Printf(proxy_h_code, "%s", documentation);
        
        Printv(proxy_h_code, function_decl, "\n", NIL);
        Printv(proxy_mm_code, function_defn, "\n", NIL);
    }
    Delete(documentation);
    //Delete(paramstring);
    Delete(proxyfunctionname);
    Delete(objcrettype);
    Delete(imcall);
    Delete(function_decl);
    Delete(function_defn);
}


/* -----------------------------------------------------------------------------
 * emitProxyClassFunction()
 *
 * Function called for writing an Objective-C proxy function for static and non-static
 * C++ class functions, static and non-static member variables, member constants/enums.
 *
 * C++ class static functions map to Objective-C "+" functions.
 * C++ class non-static functions map to Objective-C "-" functions.
 * ----------------------------------------------------------------------------- */

void OBJECTIVEC::emitProxyClassFunction(Node *n) {
    if (!proxy_flag)
        return;
    
    // Get some useful attributes of this function
    String *symname = Getattr(n, "sym:name");
    String *name = Getattr(n, "name");
    SwigType *type = Getattr(n, "type");
    ParmList *parmlist = Getattr(n, "parms");
    String *crettype = SwigType_str(type, 0);
    String *objcrettype = NewString("");
    String *imcall = NewString("");
    String *function_defn = NewString("");
    String *function_decl = NewString("");
    String *tm;
    
    String *imfunctionname = Getattr(n, "imfunctionname");
    String *proxyfunctionname;
    
    bool setter_flag = false;
    bool static_flag = (static_member_func_flag || static_member_variable_flag);
    
    // Retrieve the ObjectiveC return type for this function
    if ((tm = Swig_typemap_lookup("objctype", n, "", 0))) {
        substituteClassname(tm, type);
        Printf(objcrettype, "%s", tm);
    } else {
        Swig_warning(WARN_OBJC_TYPEMAP_OBJCTYPE_UNDEF, input_file, line_number, "No objctype typemap defined for %s\n", crettype);
    }
    
    if (member_variable_flag || static_member_variable_flag || member_constant_flag) {
        proxyfunctionname = NewString("");
        setter_flag = Cmp(symname, Swig_name_set(getNSpace(), Swig_name_member(0, proxy_class_name, variable_name))) == 0;
        if (setter_flag)
            Printf(proxyfunctionname, "set");
        else
            Printf(proxyfunctionname, "get");
        // Capitalize the first letter in the variable to create an ObjectiveC proxy function name
        Putc(toupper((int) *Char(variable_name)), proxyfunctionname);
        Printf(proxyfunctionname, "%s", Char(variable_name) + 1);
        
    } else if (member_func_flag) {
        proxyfunctionname = Copy(proxyfuncname);
    } else {
        proxyfunctionname = Copy(proxyfuncname);
    }
    
    // Deal with overloading
    
    /* Write the proxy function declaration and definition */
    
    // Begin the first line of the function declaration
    if (static_flag)
        Printv(function_decl, "+(", objcrettype, ")", proxyfunctionname, NIL);
    else
        Printv(function_decl, "-(", objcrettype, ")", proxyfunctionname, NIL);
    
    // Prepare the call to intermediate function
    Printv(imcall, imfunctionname, "(", NIL);
    
    // Attach the non-standard typemaps to the parameter list
    Swig_typemap_attach_parms("in", parmlist, NULL);
    Swig_typemap_attach_parms("objctype", parmlist, NULL);
    Swig_typemap_attach_parms("objcin", parmlist, NULL);
    
    Parm *p;
    int i = 0;
    int gencomma = 0;
    
    for (p = parmlist; p; p = nextSibling(p), i++) {
        p = skipIgnoredArgs(p);
        SwigType *pt = Getattr(p, "type");
        String *objcparmtype = NewString("");
        
        // Get the ObjectiveC parameter type for this parameter
        if ((tm = Getattr(p, "tmap:objctype"))) {
            substituteClassname(tm, pt);
            Printf(objcparmtype, "%s", tm);
        } else {
            Swig_warning(WARN_OBJC_TYPEMAP_OBJCTYPE_UNDEF, input_file, line_number, "No objctype typemap defined for %s\n", SwigType_str(pt, 0));
        }
        
        if (gencomma)
            Printf(imcall, ", ");
        
        String *arg = makeParameterName(n, p, i, setter_flag);
        
        // Use typemaps to transform type used in Objective-C proxy function to the one used in intermediate code.
        if ((tm = Getattr(p, "tmap:objcin"))) {
            substituteClassname(tm, pt);
            Replaceall(tm, "$objcinput", arg);
            Printv(imcall, tm, NIL);
        } else {
            Swig_warning(WARN_OBJC_TYPEMAP_OBJCIN_UNDEF, input_file, line_number, "No objcin typemap defined for %s\n", SwigType_str(pt, 0));
        }
        
        // Add parameter to proxy function
        if (static_flag) {
            if (gencomma == 0)
                Printf(function_decl, ": (%s)%s", objcparmtype, arg);
            else if (gencomma >= 1)
                Printf(function_decl, " %s: (%s)%s", arg, objcparmtype, arg);
        } else {
            if (gencomma == 1)
                Printf(function_decl, ": (%s)%s", objcparmtype, arg);
            else if (gencomma >= 2)
                Printf(function_decl, " %s: (%s)%s", arg, objcparmtype, arg);
        }
        gencomma++;
        
        Delete(arg);
        Delete(objcparmtype);
    }
    
    // First line of function definition
    Printv(function_defn, function_decl, "\n{\n", NIL);
    Printf(function_decl, ";");
    
    // End the call to the intermediate function
    Printv(imcall, ")", NIL);
    
    // Transform return type used in low level accessor to type used in Objective-C proxy function
    if ((tm = Swig_typemap_lookup("objcout", n, "", 0))) {
        substituteClassname(tm, type);
        Replaceall(tm, "$imcall", imcall);
    } else {
        Swig_warning(WARN_OBJC_TYPEMAP_OBJCOUT_UNDEF, input_file, line_number, "No objcout typemap defined for %s\n", crettype);
    }
    if (GetFlag(n, "feature:new"))
        Replaceall(tm, "$owner", "true");
    else
        Replaceall(tm, "$owner", "false");
    substituteClassname(tm, type);
    Printf(function_defn, " %s\n}\n", tm ? ((const String *) tm) : empty_string);
    
    // Write documentation
    if (doxygen && doxygenTranslator->hasDocumentation(n)){
        String *doxygen_comments = doxygenTranslator->getDocumentation(n);
        if(comment_creation_chatter)
            Printf(proxy_class_function_decls, "/* This was generated from emitProxyClassFunction() */");
        Printv(proxy_class_function_decls, Char(doxygen_comments), NIL);
        Delete(doxygen_comments);
    }
    
    /* Write the function declaration to the proxy_class_function_decls
     and function definition to the proxy_class_function_defns */
    Printv(proxy_class_function_decls, function_decl, "\n", NIL);
    Printv(proxy_class_function_defns, function_defn, "\n", NIL);
    
    //Delete(paramstring);
    Delete(proxyfunctionname);
    Delete(objcrettype);
    Delete(imcall);
    Delete(function_decl);
    Delete(function_defn);
}

/* -----------------------------------------------------------------------------
 * emitProxyClassConstructor()
 *
 * Function called for writing an Objective-C proxy class constructor a.k.a
 * init function(s).
 *
 * ----------------------------------------------------------------------------- */

void OBJECTIVEC::emitProxyClassConstructor(Node *n) {
    if (!proxy_flag)
        return;
    
    // Get some useful attributes of the constructor
    String *name = Getattr(n, "name");
    ParmList *parmlist = Getattr(n, "parms");
    String *imcall = NewString("");
    String *directorconnect = NewString("");
    String *constructor_defn = NewString("");
    String *constructor_decl = NewString("");
    String *tm;
    String *objcrettype = NewString("id");
    
    String *imfunctionname = Getattr(n, "imfunctionname");
    String *proxyfunctionname = NewString("init");
    bool feature_director = Swig_directorclass(n) ? true : false;

    // Deal with overloading
    
    /* Write the proxy class constructor declaration and definition */
    
    // Begin the first line of the constructor
    Printv(constructor_decl, "-(", objcrettype, ")", proxyfunctionname, NIL);
    
    // Prepare the call to intermediate function
    Printv(imcall, imfunctionname, "(", NIL);
    
    // Attach the non-standard typemaps to the parameter list
    Swig_typemap_attach_parms("in", parmlist, NULL);
    Swig_typemap_attach_parms("objctype", parmlist, NULL);
    Swig_typemap_attach_parms("objcin", parmlist, NULL);
    
    Parm *p;
    int i = 0;
    int gencomma = 0;
    
    for (p = parmlist; p; p = nextSibling(p), i++) {
        p = skipIgnoredArgs(p);
        SwigType *pt = Getattr(p, "type");
        String *objcparmtype = NewString("");
        
        // Get the ObjectiveC parameter type for this parameter
        if ((tm = Getattr(p, "tmap:objctype"))) {
            substituteClassname(tm, pt);
            Printf(objcparmtype, "%s", tm);
        } else {
            Swig_warning(WARN_OBJC_TYPEMAP_OBJCTYPE_UNDEF, input_file, line_number, "No objctype typemap defined for %s\n", SwigType_str(pt, 0));
        }
        
        if (gencomma)
            Printf(imcall, ", ");
        
        String *arg = makeParameterName(n, p, i, false);
        
        // Use typemaps to transform type used in Objective-C proxy function to the one used in intermediate code.
        if ((tm = Getattr(p, "tmap:objcin"))) {
            substituteClassname(tm, pt);
            Replaceall(tm, "$objcinput", arg);
            Printv(imcall, tm, NIL);
        } else {
            Swig_warning(WARN_OBJC_TYPEMAP_OBJCIN_UNDEF, input_file, line_number, "No objcin typemap defined for %s\n", SwigType_str(pt, 0));
        }
        
        // Add parameter to proxy function
        if (gencomma >= 1) {	// Subsequent arguments.
            Printf(constructor_decl, " %s:", arg);
        } else {			// First valid argument, prefix it with 'With' and capitalize the first letter
            Printf(constructor_decl, "With");
            Putc(toupper((int) *Char(arg)), constructor_decl);
            Printf(constructor_decl, "%s:", (Char(arg) + 1));
        }
        
        Printf(constructor_decl, " (%s)%s", objcparmtype, arg);
        gencomma++;
        
        Delete(arg);
        Delete(objcparmtype);
        
    }
    
    // First line of function definition
    Printv(constructor_defn, constructor_decl, "\n{\n", NIL);
    Printf(constructor_decl, ";");
    
    // End the call to the intermediate function
    Printv(imcall, ")", NIL);

    // Director connection
    Printv(directorconnect, "", NIL);
    if (feature_director) {
        Node *parent = parentNode(n);
        String *classname = Swig_class_name(parent);
        String *director_connect_name = NewStringf("swigDirector_%s_Connect", classname);
        Printf(directorconnect, "if (self) %s((void *)swigCPtr, self); ", Swig_name_wrapper(director_connect_name));
        Delete(classname);
        Delete(director_connect_name);
    }
    
    // Insert the objcconstructor typemap
    Hash *attributes = NewHash();
    String *constructor_code = NewString("");
    const String *construct_tm = typemapLookup(n, "objcconstructor", name, WARN_NONE, attributes);
    if (construct_tm) {
        Printv(constructor_code, construct_tm, NIL);
        Replaceall(constructor_code, "$imcall", imcall);
        Replaceall(constructor_code, "$directorconnect", directorconnect);
    }
    Printf(constructor_defn, " %s\n}\n", constructor_code);
    
    Delete(attributes);
    Delete(constructor_code);
    
    // Write documentation
    if (doxygen && doxygenTranslator->hasDocumentation(n)){
        String *doxygen_comments = doxygenTranslator->getDocumentation(n);
        if(comment_creation_chatter)
            Printf(proxy_class_function_decls, "/* This was generated from emitProxyClassConstructor() */");
        Printv(proxy_class_function_decls, Char(doxygen_comments), NIL);
        Delete(doxygen_comments);
    }
    
    /* Write the function declaration to the proxy_class_function_decls
     and function definition to the proxy_class_function_defns */
    Printv(proxy_class_function_decls, constructor_decl, "\n", NIL);
    Printv(proxy_class_function_defns, constructor_defn, "\n", NIL);
    
    //Delete(paramstring);
    Delete(proxyfunctionname);
    Delete(objcrettype);
    Delete(imcall);
    Delete(directorconnect);
    Delete(constructor_decl);
    Delete(constructor_defn);
}


/* ---------------------------------------------------------------------------
 * emitProxyClass()
 *
 * Collects all the code fragments generated by the handler function while
 * traversing the tree from the proxy_class_* variables and writes the
 * class definition (including any additional code) to proxy_h and proxy_mm files.
 *
 * Inputs:
 *  n – The class node currently processed.
 * --------------------------------------------------------------------------- */
void OBJECTIVEC::emitProxyClass(Node *n) {
    if (!proxy_flag)
        return;
    
    SwigType *typemap_lookup_type = Getattr(n, "classtypeobj");
    
    /*
     * Handle inheriting from ObjectiveC and C++ classes.
     */
    String *baseclass = NULL;
    String *c_baseclassname = NULL;
    
    // Inheritance from pure ObjectiveC classes.
    Node *attributes = NewHash();
    const String *pure_baseclass = typemapLookup(n, "objcbase", typemap_lookup_type, WARN_NONE, attributes);
    bool purebase_replace = GetFlag(attributes, "tmap:objcbase:replace") ? true : false;
    bool purebase_notderived = GetFlag(attributes, "tmap:objcbase:notderived") ? true : false;
    bool feature_director = Swig_directorclass(n) ? true : false;
    Delete(attributes);
    
    // C++ inheritance.
    if (!purebase_replace) {
        List *baselist = Getattr(n, "bases");
        if (baselist) {
            Iterator base = First(baselist);
            while (base.item && GetFlag(base.item, "feature:ignore")) {
                base = Next(base);
            }
            if (base.item) {
                c_baseclassname = Getattr(base.item, "name");
                baseclass = Copy(createProxyName(c_baseclassname));
                base = Next(base);
                /* Warn about multiple inheritance for additional base class(es) */
                while (base.item) {
                    if (GetFlag(base.item, "feature:ignore")) {
                        base = Next(base);
                        continue;
                    }
                    String *proxyclassname = SwigType_str(Getattr(n, "classtypeobj"), 0);
                    String *baseclassname = SwigType_str(Getattr(base.item, "name"), 0);
                    Swig_warning(WARN_OBJC_MULTIPLE_INHERITANCE, Getfile(n), Getline(n),
                                 "Base %s of class %s ignored: multiple inheritance is not supported in ObjectiveC.\n", baseclassname, proxyclassname);
                    base = Next(base);
                }
            }
        }
    }
    
    bool derived = baseclass && createProxyName(c_baseclassname);
    if (derived && purebase_notderived) {
        pure_baseclass = empty_string;
    }
    const String *wanted_base = baseclass ? baseclass : pure_baseclass;
    
    if (purebase_replace) {
        wanted_base = pure_baseclass;
        derived = false;
        Delete(baseclass);
        baseclass = NULL;
        if (purebase_notderived) {
            Swig_error(Getfile(n), Getline(n),
                       "The objcbase typemap for proxy %s must contain just one of the 'replace' or 'notderived' attributes.\n", typemap_lookup_type);
        }
    } else if (Len(pure_baseclass) > 0 && Len(baseclass) > 0) {
        Swig_warning(WARN_OBJC_MULTIPLE_INHERITANCE, Getfile(n), Getline(n),
                     "Warning for %s proxy: Base class %s ignored. Multiple inheritance is not supported in ObjectiveC. "
                     "Perhaps you need one of the 'replace' or 'notderived' attributes in the objcbase typemap?\n", typemap_lookup_type, pure_baseclass);
    }
    
    /* Destructor handling */
    // If the C++ destructor is accessible (public), it is wrapped by the
    // dealloc() method in ObjectiveC. If it is not accessible, dealloc() method is written and throws an exception.
    String *destructor_decl = NewString("");
    String *destructor_defn = NewString("");
    String *directordisconnect = NewString("");
    const String *tm = NULL;
    
    String *destructor_methodname;
    String *destructor_methodmodifiers;
    attributes = NewHash();
    
    tm = typemapLookup(n, "objcdestructor", typemap_lookup_type, WARN_NONE, attributes);
    destructor_methodname = Getattr(attributes, "tmap:objcdestructor:methodname");
    destructor_methodmodifiers = Getattr(attributes, "tmap:objcdestructor:methodmodifiers");

    // Director disconnection
    Printv(directordisconnect, "", NIL);
    if (feature_director) {
        String *sym_name = Getattr(n, "sym:name");
        String *director_disconnect_name = NewStringf("swigDirector_%s_Disconnect", sym_name);
        Printf(directordisconnect, "%s((void *)swigCPtr);", Swig_name_wrapper(director_disconnect_name));
        Delete(director_disconnect_name);
    }


    
    if (tm && *Char(tm)) {
        if (!destructor_methodname) {
            Swig_error(Getfile(n), Getline(n), "No methodname attribute defined in the objcdestructor typemap for %s\n", proxy_class_name);
        }
        if (!destructor_methodmodifiers) {
            Swig_error(Getfile(n), Getline(n), "No methodmodifiers attribute defined in objcdestructor typemap for %s.\n", proxy_class_name);
        }
    }
    
    if (tm) {
        // Write the dealloc() method declaration for the proxy interface
        Printv(destructor_decl, "\n", "-(void)", destructor_methodname, ";\n", NIL);
        
        // And, the dealloc() method definition for the proxy implementation
        String *destructor_code = NewString("");
        Printv(destructor_code, tm, NIL);
        
        if (*Char(destrcutor_call)) {
            Replaceall(destructor_code, "$imcall", destrcutor_call);
        } else {
            Replaceall(destructor_code, "$imcall", "[NSException raise:@\"UnsupportedOperationException\" format: @\"%@\", \
                       @\"C++ destructor does not have public access\"]");
        }
        Replaceall(destructor_code, "$directordisconnect", directordisconnect);

        if (*Char(destructor_code)) {
            Printv(destructor_defn, "\n", "-(void)", destructor_methodname, "{\n", destructor_code, "\n}\n", NIL);
        }
        
        Delete(destructor_code);
    }
    
    
    /* Write the proxy class declaration */
    // Class modifiers.
    const String *objcinterfacemodifier = typemapLookup(n, "objcinterfacemodifier", typemap_lookup_type, WARN_OBJC_INTERFACE_MOD);
    
    // User-defined protocols.
    const String *protocols = typemapLookup(n, derived ? "objcprotocols_derived" : "objcprotocols", typemap_lookup_type, WARN_NONE);
    
    // Default interface code
    const String *objcinterfacecode;
    if (derived) {
        objcinterfacecode = typemapLookup(n, "objcinterfacecode_derived", typemap_lookup_type, WARN_NONE);
    } else {
        objcinterfacecode = typemapLookup(n, "objcinterfacecode", typemap_lookup_type, WARN_NONE);
    }
    
    // Retrieve documentation
    String* documentation = NewString("");
    if (doxygen && doxygenTranslator->hasDocumentation(n)){
        String *doxygen_comments = doxygenTranslator->getDocumentation(n);
        if(comment_creation_chatter)
            Printf(documentation, "/* This was generated from emitProxyClass() */");
        Printv(documentation, Char(doxygen_comments), NIL);
        Delete(doxygen_comments);
    }
    
    // the class interface
    String *visibility = NewString("__attribute__ ((visibility(\"default\"))) ");
    Printv(proxy_class_decl_code, proxy_class_decl_imports, proxy_class_enums_code, documentation, visibility,
           objcinterfacemodifier, " $objcclassname",
           (*Char(wanted_base) || *Char(protocols)) ? " : " : "", wanted_base,
           (*Char(wanted_base) && *Char(protocols)) ? ", " : "", protocols,
           objcinterfacecode, proxy_class_function_decls, destructor_decl, "\n", typemapLookup(n, "objcclassclose", typemap_lookup_type, WARN_NONE), "\n\n", NIL);
    Delete(visibility);
    Delete(documentation);
    
    /* Write the proxy class definition */
    // Class modifiers.
    const String *objccimplementationmodifier = typemapLookup(n, "objcimplementationmodifier", typemap_lookup_type, WARN_OBJC_IMPLEMENTATION_MOD);
    
    // Default implementationcode code
    const String *objcimplementationcode;
    if (derived) {
        objcimplementationcode = typemapLookup(n, "objcimplementationcode_derived", typemap_lookup_type, WARN_NONE);
    } else {
        objcimplementationcode = typemapLookup(n, "objcimplementationcode", typemap_lookup_type, WARN_NONE);
    }
    
    // the class implementation
    Printv(proxy_class_defn_code, "\n", proxy_class_defn_imports, objccimplementationmodifier, " $objcclassname", objcimplementationcode, "\n",
           proxy_class_function_defns, destructor_defn, "\n", typemapLookup(n, "objcclassclose", typemap_lookup_type, WARN_NONE), "\n\n", NIL);
    
    Replaceall(proxy_class_decl_code, "$objcbaseclass", proxy_class_name);
    Replaceall(proxy_class_defn_code, "$objcbaseclass", proxy_class_name);
    
    Delete(baseclass);
    Delete(directordisconnect);
    Delete(destructor_decl);
    Delete(destructor_defn);
}


/* ---------------------------------------------------------------------------
 * emitTypeWrapperClass()
 * --------------------------------------------------------------------------- */
void OBJECTIVEC::emitTypeWrapperClass(String *classname, SwigType *type) {
    Node *n = NewHash();
    Setfile(n, input_file);
    Setline(n, line_number);
    
    // Pure ObjectiveC baseclass and interfaces.
    const String *pure_baseclass = typemapLookup(n, "objcbase", type, WARN_NONE);
    const String *pure_interfaces = typemapLookup(n, "objcprotocols", type, WARN_NONE);
    
    /* Write the type wrapper class declaration */
    // Class modifiers.
    const String *objcinterfacemodifier = typemapLookup(n, "objcinterfacemodifier", type, WARN_OBJC_INTERFACE_MOD);
    
    // Default interface code
    const String *objcinterfacecode = typemapLookup(n, "objcinterfacecode", type, WARN_NONE);
    
    Printv(swigtypes_h_code, objcinterfacemodifier, " $objcclassname",
           (*Char(pure_baseclass) || *Char(pure_interfaces)) ? " : " : "", pure_baseclass,
           (*Char(pure_baseclass) && *Char(pure_interfaces)) ? ", " : "", pure_interfaces,
           objcinterfacecode, "\n", typemapLookup(n, "objcclassclose", type, WARN_NONE), "\n\n", NIL);
    
    /* Write the type wrapper class definition */
    // Class modifiers.
    const String *objccimplementationmodifier = typemapLookup(n, "objcimplementationmodifier", type, WARN_OBJC_IMPLEMENTATION_MOD);
    
    // Default implementationcode code
    const String *objcimplementationcode = typemapLookup(n, "objcimplementationcode", type, WARN_NONE);
    
    Printv(swigtypes_mm_code, "\n", objccimplementationmodifier, " $objcclassname", objcimplementationcode,
           "\n", typemapLookup(n, "objcclassclose", type, WARN_NONE), "\n\n", NIL);
    
    Replaceall(swigtypes_h_code, "$objcclassname", classname);
    Replaceall(swigtypes_mm_code, "$objcclassname", classname);
    
    Delete(n);
}

/* -----------------------------------------------------------------------------
 * pragmaDirective()
 *
 * Valid Pragmas:
 * moduleimports - import statements for the module class
 * ----------------------------------------------------------------------------- */
int OBJECTIVEC::pragmaDirective(Node *n) {
    if (!ImportMode) {
        String *lang = Getattr(n, "lang");
        String *code = Getattr(n, "name");
        String *value = Getattr(n, "value");
        
        if (Strcmp(lang, "objc") == 0) {
            String *strvalue = NewString(value);
            Replaceall(strvalue, "\\\"", "\"");
            
            if (Strcmp(code, "defnmoduleimports") == 0) {
                Delete(proxy_class_defn_imports);
                proxy_class_defn_imports = NewStringf("%s\n", Copy(strvalue));
            } else if (Strcmp(code, "declmoduleimports") == 0) {
                Delete(proxy_class_decl_imports);
                proxy_class_decl_imports = NewStringf("%s\n", Copy(strvalue));
            }
            Delete(strvalue);
        }
    }
    return Language::pragmaDirective(n);
}



/* -----------------------------------------------------------------------------
 * substituteClassname()
 *
 * Substitute $objcclassname with the proxy class name for classes/structs/unions that SWIG knows about.
 * Also substitutes enums with enum name.
 * Otherwise use the $descriptor name for the Objective-C class name. Note that the $&objcclassname substitution
 * is the same as a $&descriptor substitution, ie one pointer added to descriptor name.
 *
 * Inputs:
 *   tm - String to perform the substitution at (will usually come from a
 *        typemap.
 *   pt - The type to substitute for the variables.
 * Outputs:
 *   tm - String with the variables substituted.
 * Return:
 *   substitution_performed - flag indicating if a substitution was performed
 * ----------------------------------------------------------------------------- */

bool OBJECTIVEC::substituteClassname(String *tm, SwigType *pt) {
    bool substitution_performed = false;
    SwigType *type = Copy(SwigType_typedef_resolve_all(pt));
    SwigType *strippedtype = SwigType_strip_qualifiers(type);
    
    if (Strstr(tm, "$objcclassname_stripped")) {
        SwigType *type = Copy(strippedtype);
        while (SwigType_ispointer(type) || SwigType_isreference(type)) {
            Delete(SwigType_pop(type));
        }
        substituteClassnameVariable(tm, "$objcclassname_stripped", strippedtype);
        substitution_performed = true;
        Delete(type);
    }
    if (Strstr(tm, "$objcclassname")) {
        SwigType *type = Copy(strippedtype);
        substituteClassnameVariable(tm, "$objcclassname", type);
        substitution_performed = true;
        Delete(type);
    }
    if (Strstr(tm, "$*objcclassname")) {
        SwigType *type = Copy(strippedtype);
        Delete(SwigType_pop(type));
        substituteClassnameVariable(tm, "$*objcclassname", type);
        substitution_performed = true;
        Delete(type);
    }
    if (Strstr(tm, "$&objcclassname")) {
        SwigType *type = Copy(strippedtype);
        SwigType_add_pointer(type);
        substituteClassnameVariable(tm, "$&objcclassname", type);
        substitution_performed = true;
        Delete(type);
    }
    
    Delete(strippedtype);
    Delete(type);
    
    return substitution_performed;
}

/* -----------------------------------------------------------------------------
 * substituteClassnameVariable()
 * ----------------------------------------------------------------------------- */

void OBJECTIVEC::substituteClassnameVariable(String *tm, const char *classnamevariable, SwigType *type) {
    String *type_name;
    
    if (SwigType_isenum(type)) {
        Node *n = enumLookup(type);
        String *enum_name = Getattr(n, "sym:name");
        Node *p = parentNode(n);
        if (p && !Strcmp(nodeType(p), "class")) {
            // This is a nested enum.
            String *parent_name = Getattr(p, "sym:name");
            type_name = NewStringf("enum %s_%s", parent_name, enum_name);
        } else {
            type_name = NewStringf("enum %s", enum_name);
        }
    } else {
        String *class_name = createProxyName(type);
        if (class_name) {
            type_name = Copy(class_name);
        } else {
            // SWIG does not know anything about the type (after resolving typedefs).
            // Just mangle the type name string like $descriptor(type) would do.
            String *descriptor = NewStringf("SWIGTYPE%s", SwigType_manglestr(type));
            type_name = Copy(descriptor);
            
            // Add to hash table so that the type wrapper classes can be created later
            Setattr(unknown_types, descriptor, type);
            Delete(descriptor);
        }
    }
    Replaceall(tm, classnamevariable, type_name);
    Delete(type_name);
}

/* ---------------------------------------------------------------------
 * skipIgnoredArgs()
 *
 * --------------------------------------------------------------------- */
Parm *OBJECTIVEC::skipIgnoredArgs(Parm *p) {
    while (checkAttribute(p, "tmap:in:numinputs", "0")) {
        p = Getattr(p, "tmap:in:next");
    }
    return p;
}

/* ---------------------------------------------------------------------
 * marshalInputArgs()
 *
 * Process all of the arguments passed and convert them into C/C++
 * function arguments using the supplied typemaps.
 * --------------------------------------------------------------------- */

void OBJECTIVEC::marshalInputArgs(ParmList *parmlist, Wrapper *wrapper) {
    String *tm;
    Parm *p;
    int i = 0;
    
    for (p = parmlist; p; p = nextSibling(p), i++) {
        p = skipIgnoredArgs(p);
        SwigType *pt = Getattr(p, "type");
        
        String *arg = NewString("");
        Printf(arg, "imarg%d", i + 1);
        
        // Get the "in" typemap for this argument and add to the wrapper->code
        if ((tm = Getattr(p, "tmap:in"))) {
            Replaceall(tm, "$input", arg);
            Setattr(p, "emit:input", arg);
            Printf(wrapper->code, "%s\n", tm);
        } else {
            Swig_warning(WARN_TYPEMAP_IN_UNDEF, input_file, line_number, "Unable to use type %s as a function argument.\n", SwigType_str(pt, 0));
            p = nextSibling(p);
        }
        Delete(arg);
    }
    
}

/* ---------------------------------------------------------------------
 * makeParameterList()
 *
 * --------------------------------------------------------------------- */

void OBJECTIVEC::makeParameterList(ParmList *parmlist, Wrapper *wrapper) {
    
    // Attach the non-standard typemaps to the parameter list
    Swig_typemap_attach_parms("imtype", parmlist, wrapper);
    
    String *tm;
    Parm *p;
    int i = 0;
    int gencomma = 0;
    
    for (p = parmlist; p; p = nextSibling(p), i++) {
        p = skipIgnoredArgs(p);
        SwigType *pt = Getattr(p, "type");
        
        String *imparmtype = NewString("");
        String *arg = NewString("");
        
        Printf(arg, "imarg%d", i + 1);
        
        // Get the intermediate parameter type for this parameter
        if ((tm = Getattr(p, "tmap:imtype"))) {
            Printv(imparmtype, tm, NIL);
            if (gencomma)
                Printf(wrapper->def, ", ");
            Printv(wrapper->def, imparmtype, " ", arg, NIL);	// Add parameter to the function signature (wrapper->def)
            ++gencomma;
        } else {
            Swig_warning(WARN_OBJC_TYPEMAP_IMTYPE_UNDEF, input_file, line_number, "No imtype typemap defined for %s\n", SwigType_str(pt, 0));
            p = nextSibling(p);
        }
        
        Delete(imparmtype);
        Delete(arg);
    }
}

/* ---------------------------------------------------------------------
 * marshalOutput()
 *
 * Process the return value of the C/C++ function call
 * and convert it into the intermediate type using the
 * supplied typemaps.
 * --------------------------------------------------------------------- */

void OBJECTIVEC::marshalOutput(Node *n, String *actioncode, Wrapper *wrapper) {
    SwigType *type = Getattr(n, "type");
    // Emit the out typemap
    String *tm;
    if ((tm = Swig_typemap_lookup_out("out", n, "result", wrapper, actioncode))) {
        Replaceall(tm, "$result", "imresult");
        if (GetFlag(n, "feature:new"))
            Replaceall(tm, "$owner", "1");
        else
            Replaceall(tm, "$owner", "0");
        Printf(wrapper->code, "%s", tm);
        if (Len(tm))
            Printf(wrapper->code, "\n");
    } else {
        Swig_warning(WARN_TYPEMAP_OUT_UNDEF, input_file, line_number, "Unable to use return type %s in function %s.\n", SwigType_str(type, 0), Getattr(n, "name"));
    }
    // Emits a variable declaration for a function return value
    emit_return_variable(n, type, wrapper);
}


/* ---------------------------------------------------------------------------
 * makeParameterName()
 *
 * Inputs:
 *   n - Node
 *   p - parameter node
 *   arg_num - parameter argument number
 *   setter  - set this flag when wrapping variables
 * Return:
 *   arg - a unique parameter name
 * --------------------------------------------------------------------------- */
String *OBJECTIVEC::makeParameterName(Node *n, Parm *p, int argnumber, bool setter) const {
    String *arg = 0;
    String *pn = Getattr(p, "name");
    
    // Use C parameter name unless it is a duplicate or an empty parameter name
    int count = 0;
    ParmList *plist = Getattr(n, "parms");
    while (plist) {
        if ((Cmp(pn, Getattr(plist, "name")) == 0))
            count++;
        plist = nextSibling(plist);
    }
    String *wrn = pn ? Swig_name_warning(p, 0, pn, 0) : 0;
    arg = (!pn || (count > 1) || wrn) ? NewStringf("arg%d", argnumber) : Copy(pn);
    
    if (setter && Cmp(arg, "self") != 0) {
        // In theory, we could use the normal parameter name for setter functions.
        // Unfortunately, it is set to "Class::VariableName" for static public
        // members by the parser, which is not legal ObjectiveC syntax. Thus, we just force
        // it to "value".
        Delete(arg);
        arg = NewString("value");
    }
    
    
    return arg;
}


/* -----------------------------------------------------------------------------
 * createProxyName()
 *
 * Returns the ObjectiveC class name if a type corresponds to something wrapped with a
 * proxy class, NULL otherwise.
 * ----------------------------------------------------------------------------- */

String *OBJECTIVEC::createProxyName(SwigType *t) {	// TODO: See this once more for nspaces.
    if (proxy_flag) {
        Node *n = classLookup(t);
        if (n) {
            return Getattr(n, "sym:name");
        }
    }
    return NULL;
}

/* -----------------------------------------------------------------------------
 * typemapLookup()
 * n - for input only and must contain info for Getfile(n) and Getline(n) to work
 * tmap_method - typemap method name
 * type - typemap type to lookup
 * warning - warning number to issue if no typemaps found
 * typemap_attributes - the typemap attributes are attached to this node and will 
 *   also be used for temporary storage if non null
 * return is never NULL, unlike Swig_typemap_lookup()
 * ----------------------------------------------------------------------------- */

const String *OBJECTIVEC::typemapLookup(Node *n, const_String_or_char_ptr tmap_method, SwigType *type, int warning, Node *typemap_attributes) {
    Node *node = !typemap_attributes ? NewHash() : typemap_attributes;
    Setattr(node, "type", type);
    Setfile(node, Getfile(n));
    Setline(node, Getline(n));
    const String *tm = Swig_typemap_lookup(tmap_method, node, "", 0);
    if (!tm) {
        tm = empty_string;
        if (warning != WARN_NONE)
            Swig_warning(warning, Getfile(n), Getline(n), "No %s typemap defined for %s\n", tmap_method, SwigType_str(type, 0));
    }
    if (!typemap_attributes)
        Delete(node);
    return tm;
}

/*----------------------------------------------------------------------
 * replaceSpecialVariables()
 *--------------------------------------------------------------------*/

void OBJECTIVEC::replaceSpecialVariables(String *method, String *tm, Parm *parm) {
    (void)method;
    SwigType *type = Getattr(parm, "type");
    substituteClassname(tm, type);
}

/* -----------------------------------------------------------------------
 * doxygenComment()
 * Simply translates the doxygen comment and places it into the appropriate
 * file
 * ------------------------------------------------------------------------ */
int OBJECTIVEC::doxygenComment(Node *n){
    if (doxygen && doxygenTranslator->hasDocumentation(n)){
        String *doxygen_comments=doxygenTranslator->getDocumentation(n);
        if(comment_creation_chatter)
            Printf(structuralComments, "/* This was generated from doxygenComment() */");
        Printv(structuralComments, Char(doxygen_comments), NIL);
        Delete(doxygen_comments);
    }
    return SWIG_OK;
}

/*
 * Director code
 */

 /* ---------------------------------------------------------------
 * classDirectorMethod()
 *
 * Emit a virtual director method to pass a method call on to the
 * underlying Objc object.
 *
 * --------------------------------------------------------------- */

int OBJECTIVEC::classDirectorMethod(Node *n, Node *parent, String *super) {
    String *classname = Getattr(parent, "sym:name");
    String *c_classname = Getattr(parent, "name");
    String *name = Getattr(n, "name");
    String *symname = Getattr(n, "sym:name");
    SwigType *type = Getattr(n, "type");
    SwigType *returntype = Getattr(n, "returntype");
    String *overloaded_name = getOverloadedName(n);
    String *storage = Getattr(n, "storage");
    String *value = Getattr(n, "value");
    String *decl = Getattr(n, "decl");
    String *declaration = NewString("");
    String *tm;
    Parm *p;
    int i;
    Wrapper *w = NewWrapper();
    ParmList *l = Getattr(n, "parms");
    bool is_void = !(Cmp(returntype, "void"));
    String *qualified_return = NewString("");
    bool pure_virtual = (!(Cmp(storage, "virtual")) && !(Cmp(value, "0")));
    int status = SWIG_OK;
    bool output_director = true;
    String *dirclassname = directorClassName(parent);
    String *qualified_name = NewStringf("%s::%s", dirclassname, name);
    String *jnidesc = NewString("");
    String *classdesc = NewString("");
    String *classret_desc = NewString("");
    SwigType *c_ret_type = NULL;
    String *jupcall_args = NewString("");
    String *imclass_dmethod;
    String *callback_def = NewString("");
    String *imcall_args = NewString("");
    bool ignored_method = GetFlag(n, "feature:ignore") ? true : false;
    String *qualified_classname = Copy(classname);
    String *nspace = getNSpace();

    // Kludge Alert: functionWrapper sets sym:overload properly, but it
    // isn't at this point, so we have to manufacture it ourselves. At least
    // we're consistent with the sym:overload name in functionWrapper. (?? when
    // does the overloaded method name get set?)

    imclass_dmethod = NewStringf("SwigDirector_%s", Swig_name_member(getNSpace(), classname, overloaded_name));

    if (returntype) {

        qualified_return = SwigType_rcaststr(returntype, "c_result");

        if (!is_void && (!ignored_method || pure_virtual)) {
            if (!SwigType_isclass(returntype)) {
                if (!(SwigType_ispointer(returntype) || SwigType_isreference(returntype))) {
                    String *construct_result = NewStringf("= SwigValueInit< %s >()", SwigType_lstr(returntype, 0));
                    Wrapper_add_localv(w, "c_result", SwigType_lstr(returntype, "c_result"), construct_result, NIL);
                    Delete(construct_result);
                }
                else {
                    String *base_typename = SwigType_base(returntype);
                    String *resolved_typename = SwigType_typedef_resolve_all(base_typename);
                    Symtab *symtab = Getattr(n, "sym:symtab");
                    Node *typenode = Swig_symbol_clookup(resolved_typename, symtab);

                    if (SwigType_ispointer(returntype) || (typenode && Getattr(typenode, "abstract"))) {
                        /* initialize pointers to something sane. Same for abstract
                        classes when a reference is returned. */
                        Wrapper_add_localv(w, "c_result", SwigType_lstr(returntype, "c_result"), "= 0", NIL);
                    }
                    else {
                        /* If returning a reference, initialize the pointer to a sane
                        default - if a Objc exception occurs, then the pointer returns
                        something other than a NULL-initialized reference. */
                        String *non_ref_type = Copy(returntype);

                        /* Remove reference and const qualifiers */
                        Replaceall(non_ref_type, "r.", "");
                        Replaceall(non_ref_type, "q(const).", "");
                        Wrapper_add_localv(w, "result_default", "static", SwigType_str(non_ref_type, "result_default"), "=", SwigType_str(non_ref_type, "()"), NIL);
                        Wrapper_add_localv(w, "c_result", SwigType_lstr(returntype, "c_result"), "= &result_default", NIL);

                        Delete(non_ref_type);
                    }

                    Delete(base_typename);
                    Delete(resolved_typename);
                }
            }
            else {
                SwigType *vt;

                vt = cplus_value_type(returntype);
                if (!vt) {
                    Wrapper_add_localv(w, "c_result", SwigType_lstr(returntype, "c_result"), NIL);
                }
                else {
                    Wrapper_add_localv(w, "c_result", SwigType_lstr(vt, "c_result"), NIL);
                    Delete(vt);
                }
            }
        }

        Parm *retpm = NewParm(returntype, empty_string, n);

        if ((c_ret_type = Swig_typemap_lookup("imtype", retpm, "", 0))) {
            if (!is_void && !ignored_method) {
                String *jretval_decl = NewStringf("%s jresult", c_ret_type);
                Wrapper_add_localv(w, "jresult", jretval_decl, "= 0", NIL);
                Delete(jretval_decl);
            }
        }
        else {
            Swig_warning(WARN_OBJC_TYPEMAP_OBJCTYPE_UNDEF, input_file, line_number, "No ctype typemap defined for %s for use in %s::%s (skipping director method)\n",
                SwigType_str(returntype, 0), SwigType_namestr(c_classname), SwigType_namestr(name));
            output_director = false;
        }

        Delete(retpm);
    }

    Swig_director_parms_fixup(l);

    /* Attach the standard typemaps */
    Swig_typemap_attach_parms("out", l, 0);
    Swig_typemap_attach_parms("imtype", l, 0);
    Swig_typemap_attach_parms("objctype", l, 0);
    Swig_typemap_attach_parms("directorin", l, 0);
    Swig_typemap_attach_parms("objcdirectorin", l, 0);
    Swig_typemap_attach_parms("directorargout", l, w);

    if (!ignored_method) {
        /* Build method signature */
        String *method_signature = NewString("");
        
        Printf(method_signature, "%s", symname);
        for (i = 0, p = l; p; ++i) {
            /* Is this superfluous? */
            while (checkAttribute(p, "tmap:directorin:numinputs", "0")) {
                p = Getattr(p, "tmap:directorin:next");
            }

            if (i == 0) {
                Printf(method_signature, ":");
            }
            else {
                String *argname = makeParameterName(n, p, i, false);
                Printf(method_signature, "%s:", argname);
                Delete(argname);
            }

            p = nextSibling(p);
        }

        /* Preamble code */
        Printf(w->code, "::id swigjobj = swig_get_self();\n");
        Printf(w->code, "BOOL swigmethodoverridden = NO;\n");
        Printf(w->code, "if (swigjobj) {\n");
        Printf(w->code, "  swigmethodoverridden = [swigjobj methodForSelector:@selector(%s)] != [objc_getClass(\"%s\") instanceMethodForSelector:@selector(%s)];\n", method_signature, classname, method_signature);
        Printf(w->code, "}\n");
        Printf(w->code, "if (!swigmethodoverridden) {\n");

        Delete(method_signature);
    }

    if (!pure_virtual) {
        String *super_call = Swig_method_call(super, l);
        if (is_void) {
            Printf(w->code, "%s;\n", super_call);
            if (!ignored_method)
                Printf(w->code, "return;\n");
        }
        else {
            Printf(w->code, "return %s;\n", super_call);
        }
        Delete(super_call);
    }
    else {
        Printf(w->code, "SWIG_ObjcThrowException(SWIG_ObjcDirectorPureVirtual, ");
        Printf(w->code, "\"Attempted to invoke pure virtual method %s::%s.\");\n", SwigType_namestr(c_classname), SwigType_namestr(name));

        /* Make sure that we return something in the case of a pure
        * virtual method call for syntactical reasons. */
        if (!is_void)
            Printf(w->code, "return %s;", qualified_return);
        else if (!ignored_method)
            Printf(w->code, "return;\n");
    }

    if (!ignored_method) {
        Printf(w->code, "}\n");
        Printf(w->code, "if (swigjobj) {\n");
    }

    /* Go through argument list, convert from native to Objc */
    for (i = 0, p = l; p; ++i) {
        /* Is this superfluous? */
        while (checkAttribute(p, "tmap:directorin:numinputs", "0")) {
            p = Getattr(p, "tmap:directorin:next");
        }

        SwigType *pt = Getattr(p, "type");
        String *ln = makeParameterName(n, p, i, false);
        String *c_param_type = NULL;
        String *c_decl = NewString("");
        String *arg = NewString("");
        String *arg_cast = NewString("");

        Printf(arg, "j%s", ln);

        // Get the ObjectiveC parameter type for this parameter
        if ((tm = Getattr(p, "tmap:objctype"))) {
            substituteClassname(tm, pt);
            Printf(arg_cast, "(%s)", tm);
        }
        else {
            Swig_warning(WARN_OBJC_TYPEMAP_OBJCTYPE_UNDEF, input_file, line_number, "No objctype typemap defined for %s\n", SwigType_str(pt, 0));
        }

        
        /* Get parameter's intermediary C type */
        if ((c_param_type = Getattr(p, "tmap:imtype"))) {
            String *ctypeout = Getattr(p, "tmap:imtype:out");	// the type in the ctype typemap's out attribute overrides the type in the typemap
            if (ctypeout)
                c_param_type = ctypeout;

            /* Add to local variables */
            Printf(c_decl, "%s %s", c_param_type, arg);
            if (!ignored_method)
                Wrapper_add_localv(w, arg, c_decl, (!(SwigType_ispointer(pt) || SwigType_isreference(pt)) ? "" : "= 0"), NIL);

            /* Add input marshalling code */
            if ((tm = Getattr(p, "tmap:directorin"))) {

                Setattr(p, "emit:directorinput", arg);
                Replaceall(tm, "$input", arg);
                Replaceall(tm, "$owner", "0");

                if (Len(tm))
                    if (!ignored_method)
                    Printf(w->code, "%s\n", tm);

                Delete(tm);

                /* Add parameter to the intermediate class code if generating the
                * intermediate's upcall code */
                if ((tm = Getattr(p, "tmap:imtype"))) {

                    String *imtypeout = Getattr(p, "tmap:imtype:out");	// the type in the imtype typemap's out attribute overrides the type in the typemap
                    if (imtypeout)
                        tm = imtypeout;
                    const String *im_directorinattributes = Getattr(p, "tmap:imtype:directorinattributes");

                    String *din = Copy(Getattr(p, "tmap:objcdirectorin"));

                    if (din) {
                        substituteClassname(din, pt);
                        Replaceall(din, "$iminput", arg);

                        /* Get the ObjC parameter type */
                        if ((tm = Getattr(p, "tmap:objctype"))) {
                            Replaceall(din, "$objctype", tm);
                        }
                        else {
                            Swig_warning(WARN_OBJC_TYPEMAP_OBJCTYPE_UNDEF, input_file, line_number, "No objctype typemap defined for %s\n", SwigType_str(pt, 0));
                        }

                        /* And add to the upcall args */
                        if (i == 0) {
                            Printf(jupcall_args, ": %s", din);
                        }
                        else {
                            String *argname = makeParameterName(n, p, i, false);
                            Printf(jupcall_args, " %s: %s", argname, din);
                            Delete(argname);
                        }
                    }
                    else {
                        Swig_warning(WARN_OBJC_TYPEMAP_OBJCTYPE_UNDEF, input_file, line_number, "No objcdirectorin typemap defined for %s for use in %s::%s (skipping director method)\n",
                            SwigType_str(pt, 0), SwigType_namestr(c_classname), SwigType_namestr(name));
                        output_director = false;
                    }
                }
                else {
                    Swig_warning(WARN_OBJC_TYPEMAP_OBJCTYPE_UNDEF, input_file, line_number, "No imtype typemap defined for %s for use in %s::%s (skipping director method)\n",
                        SwigType_str(pt, 0), SwigType_namestr(c_classname), SwigType_namestr(name));
                    output_director = false;
                }

                p = Getattr(p, "tmap:directorin:next");

            }
            else {
                Swig_warning(WARN_OBJC_TYPEMAP_OBJCTYPE_UNDEF, input_file, line_number,
                    "No or improper directorin typemap defined for argument %s for use in %s::%s (skipping director method)\n",
                    SwigType_str(pt, 0), SwigType_namestr(c_classname), SwigType_namestr(name));
                p = nextSibling(p);
                output_director = false;
            }
        }
        else {
            Swig_warning(WARN_OBJC_TYPEMAP_OBJCTYPE_UNDEF, input_file, line_number, "No ctype typemap defined for %s for use in %s::%s (skipping director method)\n",
                SwigType_str(pt, 0), SwigType_namestr(c_classname), SwigType_namestr(name));
            output_director = false;
            p = nextSibling(p);
        }

        Delete(ln);
        Delete(arg_cast);
        Delete(arg);
        Delete(c_decl);
        Delete(c_param_type);
    }

    /* header declaration, start wrapper definition */
    String *target;
    SwigType *rtype = Getattr(n, "conversion_operator") ? 0 : type;
    target = Swig_method_decl(rtype, decl, qualified_name, l, 0, 0);
    Printf(w->def, "%s", target);
    Delete(qualified_name);
    Delete(target);
    target = Swig_method_decl(rtype, decl, name, l, 0, 1);
    Printf(declaration, "    virtual %s", target);
    Delete(target);

    Append(w->def, " {");
    Append(declaration, ";\n");

    /* Emit the intermediate class's upcall to the actual class */

    if (!is_void) {
        Parm *tp = NewParm(returntype, empty_string, n);

        Delete(tp);
    }

    /* Finish off the inherited upcall's definition */
    Putc(')', callback_def);
    Printf(callback_def, " {\n");

    if (!ignored_method) {
        String *methid = symname;

        String *upcall = NewStringf("[swigjobj %s%s]", methid, jupcall_args);
        if (!is_void) {
            String *objcresult = NewString("objc_result");

            String *objcrettype = NewString("");
            if ((tm = Swig_typemap_lookup("objctype", n, "", 0))) {
                substituteClassname(tm, type);
                Printf(objcrettype, "%s", tm);
            }

            /* Note: it is important to keep objcresult alive until the end of scope, thus we need assignment here */
            Printf(w->code, "%s %s = %s;\n", objcrettype, objcresult, upcall);

            Parm *tp = NewParm(returntype, empty_string, n);

            if ((tm = Swig_typemap_lookup("objcdirectorout", tp, "", 0))) {
                substituteClassname(tm, returntype);
                Replaceall(tm, "$objccall", objcresult);
                Printf(w->code, "jresult = %s;\n", tm);
            }

            Delete(tm);
            Delete(tp);
            Delete(objcrettype);
            Delete(objcresult);
        }
        else {
            Printf(w->code, "%s;\n", upcall);
        }
        Delete(upcall);

        if (!is_void) {

            String *jresult_str = NewString("jresult");
            String *result_str = NewString("c_result");
            Parm *tp = NewParm(returntype, result_str, n);

            /* Copy jresult into c_result... */
            if ((tm = Swig_typemap_lookup("directorout", tp, result_str, w))) {
                Replaceall(tm, "$input", jresult_str);
                Replaceall(tm, "$result", result_str);
                Printf(w->code, "%s\n", tm);
            }
            else {
                Swig_warning(WARN_TYPEMAP_DIRECTOROUT_UNDEF, input_file, line_number,
                    "Unable to use return type %s used in %s::%s (skipping director method)\n",
                    SwigType_str(returntype, 0), SwigType_namestr(c_classname), SwigType_namestr(name));
                output_director = false;
            }

            Delete(tp);
            Delete(jresult_str);
            Delete(result_str);
        }

        /* Marshal outputs */
        for (p = l; p;) {
            if ((tm = Getattr(p, "tmap:directorargout"))) {
                Replaceall(tm, "$result", "jresult");
                Replaceall(tm, "$input", Getattr(p, "emit:directorinput"));
                Printv(w->code, tm, "\n", NIL);
                p = Getattr(p, "tmap:directorargout:next");
            }
            else {
                p = nextSibling(p);
            }
        }

        /* Terminate wrapper code */
        Printf(w->code, "} else {\n");
        Printf(w->code, "SWIG_ObjcThrowException(SWIG_ObjcRuntimeException, \"null upcall object\");\n");
        Printf(w->code, "}\n");

        if (!is_void)
            Printf(w->code, "return %s;", qualified_return);
    }

    Printf(w->code, "}");

    // We expose virtual protected methods via an extra public inline method which makes a straight call to the wrapped class' method
    String *inline_extra_method = NewString("");
    if (dirprot_mode() && !is_public(n) && !pure_virtual) {
        Printv(inline_extra_method, declaration, NIL);
        String *extra_method_name = NewStringf("%sSwigPublic", name);
        Replaceall(inline_extra_method, name, extra_method_name);
        Replaceall(inline_extra_method, ";\n", " {\n      ");
        if (!is_void)
            Printf(inline_extra_method, "return ");
        String *methodcall = Swig_method_call(super, l);
        Printv(inline_extra_method, methodcall, ";\n    }\n", NIL);
        Delete(methodcall);
        Delete(extra_method_name);
    }

    /* emit the director method */
    if (status == SWIG_OK && output_director) {
        if (!is_void) {
            Replaceall(w->code, "$null", qualified_return);
        }
        else {
            Replaceall(w->code, "$null", "");
        }
        if (!Getattr(n, "defaultargs")) {
            Replaceall(w->code, "$symname", symname);
            Wrapper_print(w, f_directors);
            Printv(f_directors_h, declaration, NIL);
            Printv(f_directors_h, inline_extra_method, NIL);
        }
    }

    Delete(inline_extra_method);
    Delete(qualified_return);
    Delete(jnidesc);
    Delete(c_ret_type);
    Delete(declaration);
    Delete(callback_def);
    Delete(overloaded_name);
    DelWrapper(w);

    return status;
}

 /*----------------------------------------------------------------------
 * emitDirectorExtraMethods()
 *
 * This is where the director connect method is generated.
 *--------------------------------------------------------------------*/
void OBJECTIVEC::emitDirectorExtraMethods(Node *n) {
    if (!Swig_directorclass(n))
        return;

    // Output the director connect method:
    String *norm_name = SwigType_namestr(Getattr(n, "name"));
    String *sym_name = Getattr(n, "sym:name");
    String *director_connect_name = NewStringf("swigDirector_%s_Connect", sym_name);
    String *director_disconnect_name = NewStringf("swigDirector_%s_Disconnect", sym_name);
    String *swig_director_connect = Swig_name_wrapper(director_connect_name);
    String *swig_director_disconnect = Swig_name_wrapper(director_disconnect_name);
    String *dirClassName = directorClassName(n);
    String *smartptr = Getattr(n, "feature:smartptr");
    
    Delete(director_connect_name);
    Delete(director_disconnect_name);

    Printf(wrap_mm_code, "void %s(void* objarg, id objcdirector) {\n", swig_director_connect);
    if (Len(smartptr)) {
        Printf(wrap_mm_code, "  %s *obj = (%s *)objarg;\n", smartptr, smartptr);
        Printf(wrap_mm_code, "  // Keep a local instance of the smart pointer around while we are using the raw pointer\n");
        Printf(wrap_mm_code, "  // Avoids using smart pointer specific API.\n");
        Printf(wrap_mm_code, "  %s *director = dynamic_cast<%s *>(obj->operator->());\n", dirClassName, dirClassName);
    }
    else {
        Printf(wrap_mm_code, "  %s *obj = *((%s **)&objarg);\n", norm_name, norm_name);
        Printf(wrap_mm_code, "  SwigDirector_%s *director = dynamic_cast<SwigDirector_%s *>(obj);\n", sym_name, sym_name);
    }
    Printf(wrap_mm_code, "  if (director) {\n");
    Printf(wrap_mm_code, "    director->swig_connect_director(objcdirector);\n");
    Printf(wrap_mm_code, "  }\n");
    Printf(wrap_mm_code, "}\n");
    Printf(wrap_mm_code, "\n");

    Printf(wrap_mm_code, "void %s(void* objarg) {\n", swig_director_disconnect);
    if (Len(smartptr)) {
        Printf(wrap_mm_code, "  %s *obj = (%s *)objarg;\n", smartptr, smartptr);
        Printf(wrap_mm_code, "  // Keep a local instance of the smart pointer around while we are using the raw pointer\n");
        Printf(wrap_mm_code, "  // Avoids using smart pointer specific API.\n");
        Printf(wrap_mm_code, "  %s *director = dynamic_cast<%s *>(obj->operator->());\n", dirClassName, dirClassName);
    }
    else {
        Printf(wrap_mm_code, "  %s *obj = *((%s **)&objarg);\n", norm_name, norm_name);
        Printf(wrap_mm_code, "  SwigDirector_%s *director = dynamic_cast<SwigDirector_%s *>(obj);\n", sym_name, sym_name);
    }
    Printf(wrap_mm_code, "  if (director) {\n");
    Printf(wrap_mm_code, "    director->swig_disconnect_director();\n");
    Printf(wrap_mm_code, "  }\n");
    Printf(wrap_mm_code, "}\n");
    Printf(wrap_mm_code, "\n");

    Delete(swig_director_connect);
    Delete(swig_director_disconnect);
    Delete(norm_name);
}

/* ------------------------------------------------------------
* classDirectorConstructor()
* ------------------------------------------------------------ */

int OBJECTIVEC::classDirectorConstructor(Node *n) {
    Node *parent = parentNode(n);
    String *decl = Getattr(n, "decl");
    String *supername = Swig_class_name(parent);
    String *classname = directorClassName(parent);
    String *sub = NewString("");
    Parm *p;
    ParmList *superparms = Getattr(n, "parms");
    ParmList *parms;
    int argidx = 0;

    /* Assign arguments to superclass's parameters, if not already done */
    for (p = superparms; p; p = nextSibling(p)) {
        String *pname = Getattr(p, "name");

        if (!pname) {
            pname = NewStringf("arg%d", argidx++);
            Setattr(p, "name", pname);
        }
    }

    /* insert jenv prefix argument */
    parms = CopyParmList(superparms);

    if (!Getattr(n, "defaultargs")) {
        /* constructor */
        {
            String *basetype = Getattr(parent, "classtype");
            String *target = Swig_method_decl(0, decl, classname, parms, 0, 0);
            String *call = Swig_csuperclass_call(0, basetype, superparms);
            String *classtype = SwigType_namestr(Getattr(n, "name"));

            Printf(f_directors, "%s::%s : %s, %s {\n", classname, target, call, Getattr(parent, "director:ctor"));
            Printf(f_directors, "}\n\n");

            Delete(classtype);
            Delete(target);
            Delete(call);
        }

        /* constructor header */
        {
            String *target = Swig_method_decl(0, decl, classname, parms, 0, 1);
            Printf(f_directors_h, "    %s;\n", target);
            Delete(target);
        }
    }

    Delete(sub);
    Delete(supername);
    Delete(parms);
    return Language::classDirectorConstructor(n);
}

/* ------------------------------------------------------------
* classDirectorDefaultConstructor()
* ------------------------------------------------------------ */

int OBJECTIVEC::classDirectorDefaultConstructor(Node *n) {
    String *classname = Swig_class_name(n);
    String *classtype = SwigType_namestr(Getattr(n, "name"));
    Wrapper *w = NewWrapper();

    Printf(w->def, "SwigDirector_%s::SwigDirector_%s() : %s {", classname, classname, Getattr(n, "director:ctor"));
    Printf(w->code, "}\n");
    Wrapper_print(w, f_directors);

    Printf(f_directors_h, "    SwigDirector_%s();\n", classname);
    DelWrapper(w);
    Delete(classtype);
    Delete(classname);
    return Language::classDirectorDefaultConstructor(n);
}


/* ------------------------------------------------------------
* classDirectorInit()
* ------------------------------------------------------------ */

int OBJECTIVEC::classDirectorInit(Node *n) {
    Delete(none_comparison);
    none_comparison = NewString("");	// not used

    Delete(director_ctor_code);
    director_ctor_code = NewString("$director_new");

    Objc_director_declaration(n);

    Printf(f_directors_h, "%s {\n", Getattr(n, "director:decl"));
    Printf(f_directors_h, "\npublic:\n");
    Printf(f_directors_h, "    void swig_connect_director(id objcdirector);\n");
    Printf(f_directors_h, "    void swig_disconnect_director();\n");

    return Language::classDirectorInit(n);
}

/* ----------------------------------------------------------------------
* classDirectorDestructor()
* ---------------------------------------------------------------------- */

int OBJECTIVEC::classDirectorDestructor(Node *n) {
    Node *current_class = getCurrentClass();
    String *full_classname = Getattr(current_class, "name");
    String *classname = Swig_class_name(current_class);
    Wrapper *w = NewWrapper();

    if (Getattr(n, "throw")) {
        Printf(f_directors_h, "    virtual ~SwigDirector_%s() throw ();\n", classname);
        Printf(w->def, "SwigDirector_%s::~SwigDirector_%s() throw () {\n", classname, classname);
    }
    else {
        Printf(f_directors_h, "    virtual ~SwigDirector_%s();\n", classname);
        Printf(w->def, "SwigDirector_%s::~SwigDirector_%s() {\n", classname, classname);
    }

    /* Ensure that correct directordisconnect typemap's method name is called
    * here: */

    Node *disconn_attr = NewHash();
    String *disconn_methodname = NULL;

    typemapLookup(n, "directordisconnect", full_classname, WARN_NONE, disconn_attr);
    disconn_methodname = Getattr(disconn_attr, "tmap:directordisconnect:methodname");

    if (disconn_methodname) {
        Printv(w->code, "  swig_disconnect_director_self(\"", disconn_methodname, "\");\n", "}", NIL);
    }
    else {
        Printv(w->code, "}", NIL);
    }

    Wrapper_print(w, f_directors);

    DelWrapper(w);
    Delete(disconn_attr);
    Delete(classname);
    return SWIG_OK;
}

/* ------------------------------------------------------------
* classDirectorEnd()
* ------------------------------------------------------------ */

int OBJECTIVEC::classDirectorEnd(Node *n) {
    String *classname = Getattr(n, "sym:name");
    String *director_classname = directorClassName(n);
    String *internal_classname;

    internal_classname = NewStringf("%s", classname);

    Wrapper *w = NewWrapper();
    Printf(w->def, "void %s::swig_connect_director(id objcdirector) {", director_classname);
    Printf(w->code, "swig_set_self(objcdirector);\n");
    Printf(w->code, "}\n");
    Wrapper_print(w, f_directors);
    DelWrapper(w);

    w = NewWrapper();
    Printf(w->def, "void %s::swig_disconnect_director() {", director_classname);
    Printf(w->code, "swig_set_self(nil);\n");
    Printf(w->code, "}\n");
    Wrapper_print(w, f_directors);
    DelWrapper(w);

    Printf(f_directors_h, "};\n\n");

    Delete(internal_classname);

    return Language::classDirectorEnd(n);
}

/* --------------------------------------------------------------------
* classDirectorDisown()
* ------------------------------------------------------------------*/

int OBJECTIVEC::classDirectorDisown(Node *n) {
    (void)n;
    return SWIG_OK;
}

/*----------------------------------------------------------------------
* extraDirectorProtectedCPPMethodsRequired()
*--------------------------------------------------------------------*/
bool OBJECTIVEC::extraDirectorProtectedCPPMethodsRequired() const {
    return false;
}

/*----------------------------------------------------------------------
* Objc_director_declaration()
*
* Generate the director class's declaration
* e.g. "class SwigDirector_myclass : public myclass, public Swig::Director {"
*--------------------------------------------------------------------*/

void OBJECTIVEC::Objc_director_declaration(Node *n) {
    String *base = Getattr(n, "classtype");
    String *class_ctor = NewString("Swig::Director()");

    String *classname = Swig_class_name(n);
    String *directorname = NewStringf("SwigDirector_%s", classname);
    String *declaration = Swig_class_declaration(n, directorname);

    Printf(declaration, " : public %s, public Swig::Director", base);

    // Stash stuff for later.
    Setattr(n, "director:decl", declaration);
    Setattr(n, "director:ctor", class_ctor);
}


/* -----------------------------------------------------------------------------
 * swig_objectivec()    - Instantiate module
 * ----------------------------------------------------------------------------- */

static Language *new_swig_objectivec() {
    return new OBJECTIVEC();
}
extern "C" Language *swig_objectivec(void) {
    return new_swig_objectivec();
}

/* -----------------------------------------------------------------------------
 * Static member variables
 * ----------------------------------------------------------------------------- */

// Usage message.
const char *const OBJECTIVEC::usage = (char *) "\
ObjectiveC options (available with -objc)\n\
-doxygen                    - Convert C++ doxygen comments to comments in proxy classes\n\
-debug-doxygen-parser       - Display doxygen parser module debugging information\n\
-debug-doxygen-translator   - Display doxygen translator module debugging information\n\
-noproxy                    - Do not generate proxy files (Only C wrappers will be generated) \n\
-help                       - This message \n\
\n";
