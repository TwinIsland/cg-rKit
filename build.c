/* cc build.c -o build_tool && ./build_tool
   ./build_tool              -- debug build (configure if no presets; auto-load .project_opt)
   ./build_tool --release    -- release build
   ./build_tool --run        -- run debug exe (build first if needed)
   ./build_tool --run --release -- run release exe (build first if needed)
   ./build_tool --config     -- reload .project_opt and .user_opt; prompt only if missing
   ./build_tool --config-all -- reconfigure .project_opt and .user_opt */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PRESETS_FILE       "CMakePresets.json"
#define PROJECT_OPTS_FILE  ".project_opt"
#define USER_OPTS_FILE     ".user_opt"

typedef struct { const char* flag; const char* label; } Lib;

static const Lib LIBS[] = {
    /* nlohmann/json is always included (see 0000_json.cmake) */
    {"USE_EIGEN",  "Eigen3"},
    {"USE_KNITRO", "Knitro"},
    {"USE_TBB",    "oneTBB"},
    {"USE_LIBIGL", "libigl"},
    {"USE_CGAL",   "CGAL"},
};
#define N ((int)(sizeof(LIBS) / sizeof(LIBS[0])))

typedef struct {
    int  sel[N];
    int  use_conda;
    char conda[512]; /* transient - never written to disk */
} Opts;

/* ---------- helpers ---------- */

static int file_exists(const char* path) {
    FILE* f = fopen(path, "r");
    if (f) { fclose(f); return 1; }
    return 0;
}

static void readline(char* buf, int size) {
    if (fgets(buf, size, stdin)) {
        int len = (int)strlen(buf);
        if (len > 0 && buf[len-1] == '\n') buf[len-1] = '\0';
    } else {
        buf[0] = '\0';
    }
}

static int run(const char* cmd) {
    printf("$ %s\n", cmd);
    return system(cmd);
}

/* ---------- validated prompts ---------- */

/* Returns 1 (yes) or 0 (no). Empty -> default N. */
static int ask_yesno(const char* label) {
    char line[64];
    for (;;) {
        printf("  %-12s [y/N]: ", label);
        readline(line, sizeof(line));
        if (line[0] == '\0' || strcmp(line, "n") == 0 || strcmp(line, "N") == 0) return 0;
        if (strcmp(line, "y") == 0 || strcmp(line, "Y") == 0) return 1;
        printf("    invalid - enter y or N (Enter = N)\n");
    }
}

/* Fills o->use_conda and o->conda. Accepts: empty/1/default -> default; 2/conda -> conda. */
static void ask_prefix(Opts* o) {
    char line[64];
    int len;
    for (;;) {
        printf("  choice [1/2, default 1]: ");
        readline(line, sizeof(line));
        if (line[0] == '\0' || strcmp(line, "1") == 0 || strcmp(line, "default") == 0) {
            o->use_conda = 0;
            o->conda[0]  = '\0';
            return;
        }
        if (strcmp(line, "2") == 0 || strcmp(line, "conda") == 0 || strcmp(line, "Conda") == 0) {
            o->use_conda = 1;
            for (;;) {
                printf("  conda prefix (absolute path): ");
                readline(o->conda, (int)sizeof(o->conda));
                len = (int)strlen(o->conda);
                while (len > 0 && o->conda[len-1] == '/') o->conda[--len] = '\0';
                if (len > 0) return;
                printf("    path cannot be empty\n");
            }
        }
        printf("    invalid - enter 1 (default) or 2 (conda)\n");
    }
}

/* ---------- opts serialize / deserialize ---------- */

static void save_project_opts(const Opts* o) {
    int i;
    FILE* f = fopen(PROJECT_OPTS_FILE, "w");
    if (!f) { fprintf(stderr, "warning: cannot write %s\n", PROJECT_OPTS_FILE); return; }
    for (i = 0; i < N; i++)
        fprintf(f, "%s=%s\n", LIBS[i].flag, o->sel[i] ? "ON" : "OFF");
    fclose(f);
}

static int load_project_opts(Opts* o) {
    char line[512];
    FILE* f = fopen(PROJECT_OPTS_FILE, "r");
    if (!f) return 0;
    memset(o, 0, sizeof(*o));
    while (fgets(line, sizeof(line), f)) {
        char* eq = strchr(line, '=');
        int i, len;
        char* val;
        if (!eq) continue;
        *eq = '\0';
        val = eq + 1;
        len = (int)strlen(val);
        if (len > 0 && val[len-1] == '\n') val[--len] = '\0';
        for (i = 0; i < N; i++) {
            if (strcmp(line, LIBS[i].flag) == 0) {
                o->sel[i] = (strcmp(val, "ON") == 0);
                break;
            }
        }
    }
    fclose(f);
    return 1;
}

static void save_user_opts(const Opts* o) {
    FILE* f = fopen(USER_OPTS_FILE, "w");
    if (!f) { fprintf(stderr, "warning: cannot write %s\n", USER_OPTS_FILE); return; }
    fprintf(f, "CMAKE_PREFIX_PATH=%s\n", o->use_conda ? o->conda : "");
    fclose(f);
}

static int load_user_opts(Opts* o) {
    char line[1024];
    FILE* f = fopen(USER_OPTS_FILE, "r");
    if (!f) return 0;
    while (fgets(line, sizeof(line), f)) {
        char* eq = strchr(line, '=');
        int len;
        char* val;
        if (!eq) continue;
        *eq = '\0';
        val = eq + 1;
        len = (int)strlen(val);
        if (len > 0 && val[len-1] == '\n') val[--len] = '\0';
        if (strcmp(line, "CMAKE_PREFIX_PATH") == 0) {
            if (len > 0) {
                if (len >= (int)sizeof(o->conda)) len = (int)sizeof(o->conda) - 1;
                memcpy(o->conda, val, (size_t)len);
                o->conda[len] = '\0';
                o->use_conda = 1;
            } else {
                o->conda[0] = '\0';
                o->use_conda = 0;
            }
        }
    }
    fclose(f);
    return 1;
}

static void print_libs(const Opts* o) {
    int i;
    for (i = 0; i < N; i++)
        printf("  %-12s %s\n", LIBS[i].flag, o->sel[i] ? "ON" : "OFF");
}

/* ---------- preset generation ---------- */

#define MAX_VARS 16
typedef struct { char key[64]; char val[256]; } KV;

static int fill_vars(KV vars[], const char* build_type, const char* prefix,
                     const int sel[]) {
    int n = 0, i;
    strcpy(vars[n].key, "CMAKE_BUILD_TYPE");              strcpy(vars[n].val, build_type);     n++;
    strcpy(vars[n].key, "CMAKE_EXPORT_COMPILE_COMMANDS"); strcpy(vars[n].val, "ON");           n++;
    strcpy(vars[n].key, "CMAKE_C_COMPILER");              strcpy(vars[n].val, "/usr/bin/gcc"); n++;
    strcpy(vars[n].key, "CMAKE_CXX_COMPILER");            strcpy(vars[n].val, "/usr/bin/g++"); n++;
    if (prefix && prefix[0]) {
        strcpy(vars[n].key, "CMAKE_PREFIX_PATH"); strcpy(vars[n].val, prefix); n++;
    }
    for (i = 0; i < N; i++) {
        strcpy(vars[n].key, LIBS[i].flag);
        strcpy(vars[n].val, sel[i] ? "ON" : "OFF");
        n++;
    }
    return n;
}

static void write_preset(FILE* f, const char* name, const char* display,
                          const char* bindir, const char* build_type,
                          const char* prefix, const int sel[]) {
    KV vars[MAX_VARS];
    int nv = fill_vars(vars, build_type, prefix, sel);
    int i;
    fprintf(f, "    {\n");
    fprintf(f, "      \"name\": \"%s\",\n", name);
    fprintf(f, "      \"displayName\": \"%s\",\n", display);
    fprintf(f, "      \"generator\": \"Ninja\",\n");
    fprintf(f, "      \"binaryDir\": \"${sourceDir}/%s\",\n", bindir);
    fprintf(f, "      \"cacheVariables\": {\n");
    for (i = 0; i < nv; i++)
        fprintf(f, "        \"%s\": \"%s\"%s\n",
                vars[i].key, vars[i].val, i + 1 < nv ? "," : "");
    fprintf(f, "      }\n");
    fprintf(f, "    }");
}

static void gen_presets(const Opts* o) {
    const char* prefix = o->use_conda ? o->conda : "";
    FILE* f = fopen(PRESETS_FILE, "w");
    if (!f) { fprintf(stderr, "cannot write %s\n", PRESETS_FILE); return; }
    fprintf(f, "{\n");
    fprintf(f, "  \"version\": 6,\n");
    fprintf(f, "  \"cmakeMinimumRequired\": { \"major\": 3, \"minor\": 23, \"patch\": 0 },\n");
    fprintf(f, "  \"configurePresets\": [\n");
    write_preset(f, "debug-ninja",   "Debug - Ninja",   "build/debug",   "Debug",   prefix, o->sel);
    fprintf(f, ",\n");
    write_preset(f, "release-ninja", "Release - Ninja", "build/release", "Release", prefix, o->sel);
    fprintf(f, "\n  ],\n");
    fprintf(f, "  \"buildPresets\": [\n");
    fprintf(f, "    { \"name\": \"debug\",   \"displayName\": \"Build Debug\","
               " \"configurePreset\": \"debug-ninja\" },\n");
    fprintf(f, "    { \"name\": \"release\", \"displayName\": \"Build Release\","
               " \"configurePreset\": \"release-ninja\" }\n");
    fprintf(f, "  ]\n}\n");
    fclose(f);
    printf("wrote %s\n", PRESETS_FILE);
}

/* ---------- post-configure steps ---------- */

static void copy_file(const char* src, const char* dst) {
    char buf[4096];
    size_t n;
    FILE* in  = fopen(src, "rb");
    FILE* out = fopen(dst, "wb");
    if (!in || !out) {
        fprintf(stderr, "warning: cannot copy %s -> %s\n", src, dst);
        if (in)  fclose(in);
        if (out) fclose(out);
        return;
    }
    while ((n = fread(buf, 1, sizeof(buf), in)) > 0)
        fwrite(buf, 1, n, out);
    fclose(in);
    fclose(out);
    printf("copied %s -> %s\n", src, dst);
}

static void post_configure(const Opts* o) {
    int i;
    for (i = 0; i < N; i++) {
        if (strcmp(LIBS[i].flag, "USE_KNITRO") == 0 && o->sel[i]) {
            copy_file("assets/knitro.option", "knitro.option");
            break;
        }
    }
}

/* ---------- ask helpers ---------- */

static void ask_libs(Opts* o) {
    int i;
    printf("Libraries (y/N, default: N):\n");
    for (i = 0; i < N; i++)
        o->sel[i] = ask_yesno(LIBS[i].label);
}

static void finish_configure(Opts* o) {
    save_project_opts(o);
    printf("wrote %s\n", PROJECT_OPTS_FILE);
    save_user_opts(o);
    printf("wrote %s\n", USER_OPTS_FILE);
    gen_presets(o);
    post_configure(o);
    printf("\nrun  ./build_tool           to debug-build\n"
           "run  ./build_tool --release to release-build\n");
}

static int load_or_create_project_opts(Opts* o) {
    if (load_project_opts(o)) {
        printf("loaded project-level libraries from %s:\n", PROJECT_OPTS_FILE);
        print_libs(o);
        return 1;
    }

    printf("no %s found - creating project-level options\n", PROJECT_OPTS_FILE);
    memset(o, 0, sizeof(*o));
    ask_libs(o);
    save_project_opts(o);
    printf("wrote %s\n", PROJECT_OPTS_FILE);
    return 1;
}

static void load_or_create_user_opts(Opts* o) {
    if (load_user_opts(o)) {
        printf("loaded user-level options from %s\n", USER_OPTS_FILE);
        return;
    }

    printf("no %s found - creating user-level options\n", USER_OPTS_FILE);
    printf("\nPrefix:\n  1) default\n  2) conda\n");
    ask_prefix(o);
    save_user_opts(o);
    printf("wrote %s\n", USER_OPTS_FILE);
}

/* ---------- build ---------- */

static int do_build(int release) {
    char cmd[128];
    int r;
    snprintf(cmd, sizeof(cmd), "cmake --preset %s", release ? "release-ninja" : "debug-ninja");
    r = run(cmd);
    if (r != 0) return r;
    snprintf(cmd, sizeof(cmd), "cmake --build --preset %s", release ? "release" : "debug");
    return run(cmd);
}

/* ---------- run ---------- */

#define MAX_EXES 16

static int collect_exes(const char* build_dir, char exes[][512], int max) {
    char cmd[512];
    char line[512];
    FILE* fp;
    int count = 0, len;
    snprintf(cmd, sizeof(cmd),
             "find \"%s/main\" -maxdepth 1 -type f -executable 2>/dev/null | sort",
             build_dir);
    fp = popen(cmd, "r");
    if (!fp) return 0;
    while (count < max && fgets(line, sizeof(line), fp)) {
        len = (int)strlen(line);
        if (len > 0 && line[len-1] == '\n') line[--len] = '\0';
        strcpy(exes[count++], line);
    }
    pclose(fp);
    return count;
}

static int do_run(int release) {
    const char* build_dir = release ? "build/release" : "build/debug";
    char exes[MAX_EXES][512];
    char cmd[600];
    char line[64];
    int count, choice, i;

    count = collect_exes(build_dir, exes, MAX_EXES);
    if (count == 0) {
        printf("no executables found - building first\n");
        if (do_build(release) != 0) return 1;
        count = collect_exes(build_dir, exes, MAX_EXES);
        if (count == 0) { fprintf(stderr, "no executables after build\n"); return 1; }
    }

    if (count == 1) {
        snprintf(cmd, sizeof(cmd), "%s", exes[0]);
        return run(cmd);
    }

    printf("multiple executables:\n");
    for (i = 0; i < count; i++)
        printf("  %d) %s\n", i + 1, exes[i]);
    for (;;) {
        printf("choice [1-%d]: ", count);
        readline(line, sizeof(line));
        choice = atoi(line);
        if (choice >= 1 && choice <= count) break;
        printf("  invalid - enter a number between 1 and %d\n", count);
    }
    snprintf(cmd, sizeof(cmd), "%s", exes[choice - 1]);
    return run(cmd);
}

/* ---------- main ---------- */

int main(int argc, char* argv[]) {
    int flag_release = 0, flag_config = 0, flag_config_all = 0, flag_run = 0, flag_help = 0, i;
    Opts o;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--release")    == 0) flag_release    = 1;
        if (strcmp(argv[i], "--config")     == 0) flag_config     = 1;
        if (strcmp(argv[i], "--configure")  == 0) flag_config     = 1;
        if (strcmp(argv[i], "--config-all") == 0) flag_config_all = 1;
        if (strcmp(argv[i], "--run")        == 0) flag_run        = 1;
        if (strcmp(argv[i], "-h")           == 0) flag_help       = 1;
        if (strcmp(argv[i], "--help")       == 0) flag_help       = 1;
    }

    if (flag_run)
        return do_run(flag_release);

    if (flag_help) {
        printf(
            "usage: ./build_tool [options]\n"
            "\n"
            "build:\n"
            "  (no flags)       debug build; runs configure if no CMakePresets.json\n"
            "  --release        release build\n"
            "\n"
            "run:\n"
            "  --run            run debug executable (builds first if needed)\n"
            "  --run --release  run release executable (builds first if needed)\n"
            "\n"
            "configure:\n"
            "  --config         reload %s and %s; prompt only for missing files\n"
            "  --config-all     reconfigure %s and %s from scratch\n"
            "\n",
            PROJECT_OPTS_FILE, USER_OPTS_FILE,
            PROJECT_OPTS_FILE, USER_OPTS_FILE
        );
        return 0;
    }

    if (flag_config_all) {
        /* ask libs + prefix from scratch */
        printf("=== CG-RKit MetaBuild ===\n\n");
        memset(&o, 0, sizeof(o));
        ask_libs(&o);
        printf("\nPrefix:\n  1) default\n  2) conda\n");
        ask_prefix(&o);
        finish_configure(&o);
        return 0;
    }

    if (flag_config) {
        printf("=== metabuild (config reload) ===\n\n");
        memset(&o, 0, sizeof(o));
        load_or_create_project_opts(&o);
        load_or_create_user_opts(&o);
        gen_presets(&o);
        post_configure(&o);
        return 0;
    }

    if (!file_exists(PRESETS_FILE)) {
        printf("=== metabuild ===\n\n");
        memset(&o, 0, sizeof(o));
        load_or_create_project_opts(&o);
        load_or_create_user_opts(&o);
        gen_presets(&o);
        post_configure(&o);
    }

    return do_build(flag_release);
}
