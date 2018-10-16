#include <mono/metadata/mono-config.h>
#include <mono/metadata/assembly.h>

#include <mono/jit/jit.h>

#define USE_DEFAULT_MONO_API_STRUCT
/* -*- C -*- */
/*
 * The structure below should contain pointers to all the Mono runtime functions used throughout ANY
 * of the generated C code. The reason for this is to avoid symbol load failures when the generated
 * bundle is used by a third party which embeds Mono and loads it dynamically (like
 * Xamarin.Android). Third parties should provide their own version of the structure - compatible
 * with this one. This is done this way so that we don't have to make the API here public in any way
 * or form and thus maintain freedom to break it as we see needed.
 *
 * Whenever ANY change to this structure is made, the `init_default_mono_api_struct` and
 * `validate_api_struct` functions found in `template_common.inc` MUST be updated.
 *
 * The `mkbundle_log_error` must be provided by the embedding third party in order to implement a
 * logging method specific to that third party (e.g. Xamarin.Android cannot use `fprintf` since it
 * won't show up in the logcat).
 */
typedef struct BundleMonoAPI
{
	void (*mono_register_bundled_assemblies) (const MonoBundledAssembly **assemblies);
	void (*mono_register_config_for_assembly) (const char* assembly_name, const char* config_xml);
	void (*mono_jit_set_aot_mode) (int mode);
	void (*mono_aot_register_module) (void* aot_info);
	void (*mono_config_parse_memory) (const char *buffer);
	void (*mono_register_machine_config) (const char *config_xml);
} BundleMonoAPI;

#ifdef USE_DEFAULT_MONO_API_STRUCT
#include <stdio.h>
#include <stdarg.h>

static void
mkbundle_log_error (const char *format, ...)
{
	va_list ap;

	va_start (ap, format);
	vfprintf (stderr, format, ap);
	va_end (ap);
}
#endif // USE_DEFAULT_MONO_API_STRUCT
extern const unsigned char assembly_data_Mp3ToWAV_exe [];
static const MonoBundledAssembly assembly_bundle_Mp3ToWAV_exe = {"Mp3ToWAV.exe", assembly_data_Mp3ToWAV_exe, 12800};
extern const unsigned char assembly_config_Mp3ToWAV_exe [];
extern const unsigned char assembly_data_I18N_West_dll [];
static const MonoBundledAssembly assembly_bundle_I18N_West_dll = {"I18N.West.dll", assembly_data_I18N_West_dll, 72192};
extern const unsigned char assembly_data_I18N_dll [];
static const MonoBundledAssembly assembly_bundle_I18N_dll = {"I18N.dll", assembly_data_I18N_dll, 39424};
extern const char system_config;
extern const char machine_config;

static const MonoBundledAssembly *bundled [] = {
	&assembly_bundle_Mp3ToWAV_exe,
	&assembly_bundle_I18N_West_dll,
	&assembly_bundle_I18N_dll,
	NULL
};

/* -*- C -*- */
#include <stdlib.h>

static BundleMonoAPI mono_api;

void initialize_mono_api (const BundleMonoAPI *info)
{
	if (info == NULL) {
		mkbundle_log_error ("mkbundle: missing Mono API info\n");
		exit (1);
	}

	mono_api.mono_register_bundled_assemblies = info->mono_register_bundled_assemblies;
	mono_api.mono_register_config_for_assembly = info->mono_register_config_for_assembly;
	mono_api.mono_jit_set_aot_mode = info->mono_jit_set_aot_mode;
	mono_api.mono_aot_register_module = info->mono_aot_register_module;
	mono_api.mono_config_parse_memory = info->mono_config_parse_memory;
	mono_api.mono_register_machine_config = info->mono_register_machine_config;
}

static int
validate_api_pointer (const char *name, void *ptr)
{
	if (ptr != NULL)
		return 0;

	mkbundle_log_error ("mkbundle: Mono API pointer '%s' missing\n", name);
	return 1;
}

static void
validate_api_struct ()
{
	int missing = 0;

	missing += validate_api_pointer ("mono_register_bundled_assemblies", mono_api.mono_register_bundled_assemblies);
	missing += validate_api_pointer ("mono_register_config_for_assembly", mono_api.mono_register_config_for_assembly);
	missing += validate_api_pointer ("mono_jit_set_aot_mode", mono_api.mono_jit_set_aot_mode);
	missing += validate_api_pointer ("mono_aot_register_module", mono_api.mono_aot_register_module);
	missing += validate_api_pointer ("mono_config_parse_memory", mono_api.mono_config_parse_memory);
	missing += validate_api_pointer ("mono_register_machine_config", mono_api.mono_register_machine_config);

	if (missing <= 0)
		return;

	mkbundle_log_error ("mkbundle: bundle not initialized properly, %d Mono API pointers are missing\n", missing);
	exit (1);
}

static void
init_default_mono_api_struct ()
{
#ifdef USE_DEFAULT_MONO_API_STRUCT
	mono_api.mono_register_bundled_assemblies = mono_register_bundled_assemblies;
	mono_api.mono_register_config_for_assembly = mono_register_config_for_assembly;
	mono_api.mono_jit_set_aot_mode = mono_jit_set_aot_mode;
	mono_api.mono_aot_register_module = mono_aot_register_module;
	mono_api.mono_config_parse_memory = mono_config_parse_memory;
	mono_api.mono_register_machine_config = mono_register_machine_config;
#endif // USE_DEFAULT_MONO_API_STRUCT
}

static void install_aot_modules (void) {

	mono_api.mono_jit_set_aot_mode (MONO_AOT_MODE_NORMAL);

}

static char *image_name = "Mp3ToWAV.exe";

static void install_dll_config_files (void) {

	mono_api.mono_register_config_for_assembly ("Mp3ToWAV.exe", assembly_config_Mp3ToWAV_exe);

	mono_api.mono_config_parse_memory (&system_config);

	mono_api.mono_register_machine_config (&machine_config);

}

static const char *config_dir = NULL;
void mono_mkbundle_init ()
{
	install_dll_config_files ();
	mono_api.mono_register_bundled_assemblies(bundled);

	install_aot_modules ();
}
int mono_main (int argc, char* argv[]);

#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#endif

static char **mono_options = NULL;

static int count_mono_options_args (void)
{
	const char *e = getenv ("MONO_BUNDLED_OPTIONS");
	const char *p, *q;
	int i, n;

	if (e == NULL)
		return 0;

	/* Don't bother with any quoting here. It is unlikely one would
	 * want to pass options containing spaces anyway.
	 */

	p = e;
	n = 1;
	while ((q = strchr (p, ' ')) != NULL) {
		n++;
		p = q + 1;
	}

	mono_options = malloc (sizeof (char *) * (n + 1));

	p = e;
	i = 0;
	while ((q = strchr (p, ' ')) != NULL) {
		mono_options[i] = malloc ((q - p) + 1);
		memcpy (mono_options[i], p, q - p);
		mono_options[i][q - p] = '\0';
		i++;
		p = q + 1;
	}
	mono_options[i++] = strdup (p);
	mono_options[i] = NULL;

	return n;
}


int main (int argc, char* argv[])
{
	char **newargs;
	int i, k = 0;

	newargs = (char **) malloc (sizeof (char *) * (argc + 2 + count_mono_options_args ()));

	newargs [k++] = argv [0];

	if (mono_options != NULL) {
		i = 0;
		while (mono_options[i] != NULL)
			newargs[k++] = mono_options[i++];
	}

	newargs [k++] = image_name;

	for (i = 1; i < argc; i++) {
		newargs [k++] = argv [i];
	}
	newargs [k] = NULL;
	
	if (config_dir != NULL && getenv ("MONO_CFG_DIR") == NULL)
		mono_set_dirs (getenv ("MONO_PATH"), config_dir);
	
	mono_mkbundle_init();

	return mono_main (k, newargs);
}
