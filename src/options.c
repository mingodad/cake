#include "options.h"
#include <string.h>
#include "console.h"
#include <stdio.h>
#include <assert.h>


struct diagnostic default_diagnostic = {

      .warnings = ~0ULL
};

static struct w {
    enum diagnostic_id w;
    const char* name;
}
s_warnings[] = {
    {W_UNUSED_VARIABLE, "unused-variable"},
    {W_DEPRECATED, "deprecated"},
    {W_ENUN_CONVERSION,"enum-conversion"},
    {W_NON_NULL, "nonnull"},
    {W_ADDRESS, "address"},
    {W_UNUSED_PARAMETER, "unused-parameter"},
    {W_DECLARATOR_HIDE, "hide-declarator"},
    {W_TYPEOF_ARRAY_PARAMETER, "typeof-parameter"},
    {W_ATTRIBUTES, "attributes"},
    {W_UNUSED_VALUE, "unused-value"},
    {W_STYLE, "style"},
    {W_COMMENT,"comment"},
    {W_LINE_SLICING,"line-slicing"},


    {W_DISCARDED_QUALIFIERS, "discarded-qualifiers"},
    {W_UNINITIALZED, "uninitialized"},
    {W_RETURN_LOCAL_ADDR, "return-local-addr"},
    {W_DIVIZION_BY_ZERO,"div-by-zero"},
    
    
    {W_STRING_SLICED,"string-slicing"},
    {W_DECLARATOR_STATE,"declarator-state"},
    {W_OWNERSHIP_MISSING_OWNER_QUALIFIER, "missing-owner-qualifier"},
    {W_OWNERSHIP_NOT_OWNER,"not-owner"},
    {W_OWNERSHIP_USING_TEMPORARY_OWNER,"temp-owner"},
    {W_OWNERSHIP_MOVE_ASSIGNMENT_OF_NON_OWNER, "non-owner-move"},
    {W_OWNERSHIP_NON_OWNER_TO_OWNER_ASSIGN, "non-owner-to-owner-move"},
    {W_DISCARDING_OWNER, "discard-owner"},
    {W_OWNERSHIP_FLOW_MISSING_DTOR, "missing-destructor"},
    {W_OWNERSHIP_NON_OWNER_MOVE, "non-owner-move"},
    {W_MAYBE_UNINITIALIZED, "uninitialized"},
    {W_NULL_DEREFERENCE, "analyzer-null-dereference"}, // -fanalyzer
    {W_MAYBE_NULL_TO_NON_OPT_ARGUMENT, "non-opt-arg"}
    
};

enum diagnostic_id  get_warning(const char* wname)
{

    for (int j = 0; j < sizeof(s_warnings) / sizeof(s_warnings[0]); j++)
    {
        if (strncmp(s_warnings[j].name, wname, strlen(s_warnings[j].name)) == 0)
        {
            return s_warnings[j].w;
        }
    }
    return 0;
}

unsigned long long  get_warning_bit_mask(const char* wname)
{

    for (int j = 0; j < sizeof(s_warnings) / sizeof(s_warnings[0]); j++)
    {
        if (strncmp(s_warnings[j].name, wname, strlen(s_warnings[j].name)) == 0)
        {
            return (1ULL << s_warnings[j].w);
        }
    }
    return 0;
}

const char* get_warning_name(enum diagnostic_id w)
{
    //TODO because s_warnings is out of order ....
    //this is a linear seatch instead of just index! TODOD
    for (int j = 0; j < sizeof(s_warnings) / sizeof(s_warnings[0]); j++)
    {
        if (s_warnings[j].w == w)
        {
            return s_warnings[j].name;
        }
    }
    return "";
}

int fill_options(struct options* options,
    int argc,
    const char** argv)
{

    /*
       default at this moment is same as -Wall
    */
    options->diagnostic_stack[0].warnings = ~0ULL;
    options->diagnostic_stack[0].warnings &= ~(
        (1ULL << W_NOTE) |
        (1ULL << W_STYLE) |
        (1ULL << W_UNUSED_PARAMETER) |
        (1ULL << W_UNUSED_VARIABLE)); //default is OFF
    
    
    

#ifdef __EMSCRIPTEN__
    options->flow_analysis = true;
#endif

    /*first loop used to collect options*/
    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] != '-')
            continue;

        if (argv[i][1] == 'I' ||
            argv[i][1] == 'D')
        {
            /*
              Valid, but handled with preprocessor
            */
            continue;
        }

        if (strcmp(argv[i], "-no-output") == 0)
        {
            options->no_output = true;
            continue;
        }

        if (strcmp(argv[i], "-o") == 0)
        {
            if (i + 1 < argc)
            {
                strcpy(options->output, argv[i + 1]);
                i++;
            }
            else
            {
                //ops
            }
            continue;
        }
        
        if (strcmp(argv[i], "-showIncludes") == 0)
        {
            options->show_includes = true;
            continue;
        }
        if (strcmp(argv[i], "-E") == 0)
        {
            options->preprocess_only = true;
            continue;
        }

        if (strcmp(argv[i], "-sarif") == 0)
        {
            options->sarif_output = true;
            continue;
        }

        if (strcmp(argv[i], "-analyze") == 0)
        {
            options->flow_analysis = true;
            continue;
        }

        if (strcmp(argv[i], "-remove-comments") == 0)
        {
            options->remove_comments = true;
            continue;
        }

        if (strcmp(argv[i], "-direct-compilation") == 0 ||
            strcmp(argv[i], "-rm") == 0)
        {
            options->direct_compilation = true;
            continue;
        }

        if (strcmp(argv[i], "-fi") == 0)
        {
            options->format_input = true;
            continue;
        }

        if (strcmp(argv[i], "-fo") == 0)
        {
            options->format_ouput = true;
            continue;
        }

        if (strcmp(argv[i], "-nullchecks") == 0)
        {
            options->null_checks = true;
            continue;
        }

        if (strcmp(argv[i], "-msvc-output") == 0)
        {
            options->visual_studio_ouput_format = true;
            continue;
        }


        //
        if (strcmp(argv[i], "-style=cake") == 0)
        {
            options->style = STYLE_CAKE;
            continue;
        }

        if (strcmp(argv[i], "-style=gnu") == 0)
        {
            options->style = STYLE_GNU;
            continue;
        }

        if (strcmp(argv[i], "-style=microsoft") == 0)
        {
            options->style = STYLE_GNU;
            continue;
        }

        //
        if (strcmp(argv[i], "-target=c89") == 0)
        {
            options->target = LANGUAGE_C89;
            continue;
        }

        if (strcmp(argv[i], "-target=c99") == 0)
        {
            options->target = LANGUAGE_C99;
            continue;
        }
        if (strcmp(argv[i], "-target=c11") == 0)
        {
            options->target = LANGUAGE_C11;
            continue;
        }
        if (strcmp(argv[i], "-target=c2x") == 0)
        {
            options->target = LANGUAGE_C2X;
            continue;
        }
        if (strcmp(argv[i], "-target=cxx") == 0)
        {
            options->target = LANGUAGE_CXX;
            continue;
        }



        //
        if (strcmp(argv[i], "-std=c99") == 0)
        {
            options->input = LANGUAGE_C99;
            continue;
        }
        if (strcmp(argv[i], "-std=c11") == 0)
        {
            options->input = LANGUAGE_C11;
            continue;
        }
        if (strcmp(argv[i], "-std=c2x") == 0)
        {
            options->input = LANGUAGE_C2X;
            continue;
        }
        if (strcmp(argv[i], "-std=cxx") == 0)
        {
            options->input = LANGUAGE_CXX;
            continue;
        }

        //warnings
        if (argv[i][1] == 'W')
        {
            if (strcmp(argv[i], "-Wall") == 0)
            {
                options->diagnostic_stack[0].warnings = ~0ULL;
                continue;
            }
            const bool disable_warning = (argv[i][2] == 'n' && argv[i][3] == 'o');

            enum diagnostic_id  w = 0;

            if (disable_warning)
                w =  get_warning_bit_mask(argv[i] + 5);
            else
                w =  get_warning_bit_mask(argv[i] + 2);

            if (w == 0)
            {
                printf("unknown warning '%s'", argv[i]);
                return 1;
            }


            if (disable_warning)
            {
                options->diagnostic_stack[0].warnings &= ~w;
            }
            else
            {
                options->diagnostic_stack[0].warnings |= w;
            }
            continue;
        }

        if (strcmp(argv[i], "-dump-tokens") == 0)
        {
            options->dump_tokens = true;
            continue;
        }

        if (strcmp(argv[i], "-dump-pp-tokens") == 0)
        {
            options->dump_pptokens = true;
            continue;
        }

        printf("unknown option '%s'", argv[i]);
        return 1;
    }
    return 0;
}


void print_help()
{
    const char* options =
        "cake [options] source1.c source2.c ...\n"
        "\n"
        WHITE "SAMPLES\n" RESET
        "\n"
        WHITE "    cake source.c\n" RESET
        "    Compiles source.c and outputs /out/source.c\n"
        "\n"
        WHITE "    cake -target=C11 source.c\n" RESET
        "    Compiles source.c and outputs C11 code at /out/source.c\n"
        "\n"
        WHITE "cake file.c -o file.cc && cl file.cc\n" RESET
        "    Compiles file.c and outputs file.cc then use cl to compile file.cc\n"
        "\n"
        WHITE "cake file.c -direct-compilation -o file.cc && cl file.cc\n" RESET
        "    Compiles file.c and outputs file.cc for direct compilation then use cl to compile file.cc\n"
        "\n"
        WHITE "OPTIONS\n" RESET
        "\n"
        WHITE "  -I                   " RESET " Adds a directory to the list of directories searched for include files \n"
        "                        (On windows, if you run cake at the visual studio command prompt cake \n"
        "                        uses the same include files used by msvc )\n"
        "\n"
        WHITE "  -no-output            " RESET "Cake will not generate ouput\n"
        "\n"
        WHITE "  -D                    " RESET "Defines a preprocessing symbol for a source file \n"
        "\n"
        WHITE "  -E                    " RESET "Copies preprocessor output to standard output \n"
        "\n"
        WHITE "  -o name.c             " RESET "Defines the ouput name. used when we compile one file\n"
        "\n"
        WHITE "  -remove-comments      " RESET "Remove all comments from the ouput file \n"
        "\n"
        WHITE "  -direct-compilation   " RESET "output without macros/preprocessor parts\n"
        "\n"
        WHITE "  -target=standard      " RESET "Output target C standard (c89, c99, c11, c2x, cxx) \n"
        "                        C99 is the default and C89 (ANSI C) is the minimum target \n"
        "\n"
        WHITE "  -std=standard         " RESET "Assume that the input sources are for standard (c89, c99, c11, c2x, cxx) \n"
        "                        (not implemented yet, input is considered C23)                    \n"
        "\n"
        WHITE "  -fi                   " RESET "Format input (format before language convertion)\n"
        "\n"
        WHITE "  -fo                   " RESET "Format output (format after language convertion, result parsed again)\n"
        "\n"
        WHITE "  -no-discard           " RESET "Makes [[nodiscard]] default implicitly \n"
        "\n"
        WHITE "  -Wname -Wno-name      " RESET "Enables or disable warning\n"
        "\n"
        WHITE "  -sarif                " RESET "Generates sarif files\n"
        "\n"
        WHITE "  -msvc-output          " RESET "Ouput is compatible with visual studio\n"
        "\n"
        WHITE "  -dump-tokens          " RESET "Output tokens before preprocessor\n"
        "\n"
        WHITE "  -dump-pp-tokens       " RESET "Output tokens after preprocessor\n"
        "\n"
        "More details at http://thradams.com/cake/manual.html\n"
        ;

    printf("%s", options);
}

#ifdef TEST
#include "unit_test.h"
#include <string.h>

void test_get_warning_name()
{
    const char* name = get_warning_name(W_OWNERSHIP_FLOW_MISSING_DTOR);
    assert(strcmp(name, "missing-destructor") == 0);

    unsigned long long  flags = get_warning_bit_mask(name);
    assert(flags == (1ULL << W_OWNERSHIP_FLOW_MISSING_DTOR));


        const char* name2 = get_warning_name(W_STYLE);
    assert(strcmp(name2, "style") == 0);

    unsigned long long  flags2 = get_warning_bit_mask(name2);
    assert(flags2 == (1ULL << W_STYLE));
}

#endif
