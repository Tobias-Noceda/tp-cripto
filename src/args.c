#include "args.h"

#include <argp.h>
#include <string.h>
#include <strings.h>

#include <readers.h>
#include <writers.h>

/**
 * @brief argp option parser
 * 
 * @param key The option key
 * @param arg The option argument
 * @param state The argp state
 * @return error_t 0 on success, ARGP_ERR_UNKNOWN on unknown option
 */
static error_t parse_opt(int key, char *arg, struct argp_state *state);

/**
 * @brief This are the arguments keys used by argp
 * The values are arbitrary, but must be unique
 */
static enum {
    // -embed: Embed mode
    ARGK_EMBED = 'e',
    // -extract: Extract mode
    ARGK_EXTRACT = 'x',
    // -stego: Stego method to use
    ARGK_STEGO = 's',
    // -p: Porter file
    ARGK_PORTER = 'p',
    // -in: Input file (file to embed)
    ARGK_INPUT = 'i',
    // -out: Output file (bmp when embedding, input file when extracting)
    ARGK_OUTPUT = 'o',
    // -a: Algorithm for encryption/decryption
    ARGK_ALGORITHM = 'a',
    // -m: Mode for block encryption/decryption
    ARGK_MODE = 'm',
    // -pass: Password for encryption/decryption
    ARGK_PASSWORD = 'w'
} ARGK __attribute__((unused));

/**
 * @brief The argp options structure
 */
static struct argp_option options[] = {
    {
        .name = "embed",
        .key = ARGK_EMBED,
        .arg = NULL,
        .doc = "Enable embedding mode (hide data in an image)",
    },
    {
        .name = "extract",
        .key = ARGK_EXTRACT,
        .arg = NULL,
        .doc = "Enable extraction mode (recover hidden data from an image)",
    },
    {
        .name = "stego",
        .key = ARGK_STEGO,
        .arg = "STEGO_TYPE",
        .doc = "Select steganography method to use",
    },
    {
        .name = "p",
        .key = ARGK_PORTER,
        .arg = "PORTER_FILE",
        .doc = "Porter BMP file",
    },
    {
        .name = "in",
        .key = ARGK_INPUT,
        .arg = "INPUT_FILE",
        .doc = "File to embed",
    },
    {
        .name = "out",
        .key = ARGK_OUTPUT,
        .arg = "OUTPUT_FILE",
        .doc = "Output file (with .bmp extension when embedding, without extension when extracting)",
    },
    {
        .name = "algorithm",
        .key = ARGK_ALGORITHM,
        .arg = "ALGORITHM",
        .doc = "Select encryption/decryption algorithm (AES_128, AES_192, AES_256, DES_3)",
    },
    {
        .name = "mode",
        .key = ARGK_MODE,
        .arg = "MODE",
        .doc = "Select cipher mode for encryption/decryption (ECB, CFB, OFB, CBC)",
    },
    {
        .name = "pass",
        .key = ARGK_PASSWORD,
        .arg = "PASSWORD",
        .doc = "Password for encryption/decryption",
    },
    {0},
};

static struct argp argp = {
    .options = options,
    .parser = parse_opt,
    .doc = "A steganography tool for embedding and extracting data from BMP images using LSB methods.",
};

static const StegoMethod stego_methods[] = {
    {"LSB1", embed_data_lsb1, retrieve_lsb1},
    {"LSB4", embed_data_lsb4, retrieve_lsb4},
    {0},
};

static StegoMethod get_stego_method(const char *name)
{
    size_t i;
    for (i = 0; stego_methods[i].name; i++)
        if (!strcasecmp(stego_methods[i].name, name))
            return stego_methods[i];
    return stego_methods[i];
}

static const CipherAlgoEnum cipher_algorithms[] = {
    {"aes128", AES_128},
    {"aes192", AES_192},
    {"aes256", AES_256},
    {"3des", DES_3},
    {0},
};

static CipherAlgo get_cipher_algorithm(const char *name)
{
    size_t i;
    for (i = 0; cipher_algorithms[i].name; i++)
        if (!strcasecmp(cipher_algorithms[i].name, name))
            return cipher_algorithms[i].algo;
    return -1;
}

static const CipherModeEnum cipher_modes[] = {
    {"ecb", ECB},
    {"cfb", CFB},
    {"ofb", OFB},
    {"cbc", CBC},
    {0},
};

static CipherMode get_cipher_mode(const char *name)
{
    size_t i;
    for (i = 0; cipher_modes[i].name; i++)
        if (!strcasecmp(cipher_modes[i].name, name))
            return cipher_modes[i].mode;
    return -1;
}

static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
    struct Arguments *arguments = state->input;

    switch (key)
    {
    case ARGK_EMBED:
        arguments->embed = true;
        break;

    case ARGK_EXTRACT:
        arguments->extract = true;
        break;

    case ARGK_STEGO:
        arguments->stego = get_stego_method(arg);
        if (!arguments->stego.name)
            argp_error(state, "Invalid stego method. Available methods: LSB1, LSB4, LSBI.");
        break;

    case ARGK_PORTER:
        arguments->porter_path = arg;
        break;

    case ARGK_INPUT:
        arguments->input_path = arg;
        break;

    case ARGK_OUTPUT:
        arguments->output_path = arg;
        break;

    case ARGK_ALGORITHM:
        arguments->algorithm = get_cipher_algorithm(arg);
        if (arguments->algorithm == -1) {
            argp_error(state, "Invalid cipher algorithm. Available algorithms: AES_128, AES_192, AES_256, DES_3.");

            arguments->algorithm = 0;
        }
        break;

    case ARGK_MODE:
        arguments->mode = get_cipher_mode(arg);
        if (arguments->mode == -1) {
            argp_error(state, "Invalid cipher mode. Available modes: ECB, CFB, OFB, CBC.");
        }
        break;

    case ARGK_PASSWORD:
        arguments->password = arg;
        break;

    case ARGP_KEY_ARG:
        /* No positional arguments are expected */
        argp_usage(state);
        break;

    case ARGP_KEY_END:
        /* Validation logic */
        if (arguments->embed && arguments->extract)
            argp_error(state, "Cannot use -embed and -extract together.");

        if (!arguments->embed && !arguments->extract)
            argp_error(state, "You must specify either -embed or -extract.");

        if (arguments->embed && !arguments->input_path)
            argp_error(state, "-in is required in -embed mode.");

        if (arguments->extract && arguments->input_path)
            argp_error(state, "-in cannot be used with -extract mode.");

        if (!arguments->porter_path)
            argp_error(state, "-p is required.");

        if (!arguments->output_path)
            argp_error(state, "-out is required.");

        if (!arguments->stego.name)
            argp_error(state, "-stego (LSB1 | LSB4 | LSBI) is required.");

        if (arguments->algorithm) {
            if (!arguments->password)
                argp_error(state, "-password is required when using encryption.");

            if (arguments->mode == -1)
                argp_error(state, "-mode is required when using encryption.");
        }

        if (arguments->mode || arguments->password) {
            if (arguments->algorithm == -1)
                argp_error(state, "-algorithm is required when mode or password is specified.");
        }

        break;

    default:
        return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

// Linked list methods for... something...
typedef struct Node
{
    void *ptr;
    struct Node *next;
} Node;

static bool push_ptr_list(Node **head, Node **tail, void *ptr);
static void free_ptr_list(Node *node);

static char *prepend_dash(char *arg);

Arguments get_args(int argc, char *argv[])
{
    Arguments arguments = {0};

    // Convert single-dash options to double-dash options for argp compatibility
    Node *head = NULL;
    Node *tail = NULL;

    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-' && strlen(argv[i]) > 2 && argv[i][1] != '-')
        {
            argv[i] = prepend_dash(argv[i]);
            if (!argv[i] || !push_ptr_list(&head, &tail, argv[i])) {
                free(argv[i]); // free(NULL) is safe
                free_ptr_list(head);
                exit(EXIT_FAILURE);
            }
        }
    }

    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    free_ptr_list(head);

    return arguments;
}

// Linked list of pointers that need to be freed
// Sorry :]

static void free_ptr_list(Node *node)
{
    if (!node)
        return;
    free_ptr_list(node->next);
    free(node->ptr);
    free(node);
}

static bool push_ptr_list(Node **head, Node **tail, void *ptr)
{
    Node *node = malloc(sizeof(Node));
    if (!node)
    {
        perror("Memory allocation failed");
        return false;
    }

    node->ptr = ptr;
    node->next = NULL;

    if (!*head)
        *head = node;
    else
        (*tail)->next = node;
    *tail = node;

    return true;
}

static char *prepend_dash(char *arg)
{
    char *new_arg = malloc(strlen(arg) + 2);
    if (!new_arg)
    {
        perror("Memory allocation failed");
        return NULL;
    }

    new_arg[0] = '-';
    strcpy(new_arg + 1, arg);

    return new_arg;
}
