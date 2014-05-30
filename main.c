
//
// clib-validate: `main.c`
//
// Copyright (c) 2014 Stephen Mathieson
// MIT licensed
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "str-copy/str-copy.h"
#include "path-join/path-join.h"
#include "parson/parson.h"
#include "logger/logger.h"
#include "parse-repo/parse-repo.h"

#define ERROR(err) ({         \
  rc = 1;                     \
  logger_error("error", err); \
  goto done;                  \
})

#define WARN(warning) ({           \
  rc++;                            \
  logger_warn("warning", warning); \
});

#define WARN_MISSING(key) ({                    \
  WARN("missing " #key " in package.json"); \
})

#define require_string(name) ({                                \
  const char * __## name = json_object_get_string(obj, #name); \
  if (!(__## name)) WARN_MISSING(#name);                                \
})

void
usage() {
  printf("\n");
  printf("  Usage: clib validate <dir>\n");
  printf("\n");
  printf("    Validate a package.json in the given <dir>.");
  printf("\n");
  printf("  Examples:\n");
  printf("\n");
  printf("    # validate ./package.json\n");
  printf("    $ clib validate\n");
  printf("\n");
  printf("    # validate ./repos/foo/package.json\n");
  printf("    $ clib validate repos/foo\n");
  printf("\n");
  exit(1);
}

int
main(int argc, char **argv) {
  char *directory = NULL;
  char *file = NULL;
  const char *repo = NULL;
  char *repo_owner = NULL;
  char *repo_name = NULL;
  int rc = 0;
  JSON_Value *root = NULL;
  JSON_Object *obj = NULL;
  JSON_Array *src = NULL;
  JSON_Array *keywords = NULL;

  for (int i = 1; i < argc; ++i) {
    if (0 == strncmp("--help", argv[i], 6)) usage();
    if (0 == strncmp("-h", argv[i], 2)) usage();
    if (!(directory = str_copy(argv[i]))) ERROR("out of memory");
  }

  printf("\n");

  if (!directory) {
    if (!(directory = getcwd(0, 0))) ERROR("out of memory");
  }
  if (!(file = path_join(directory, "package.json"))) ERROR("out of memory");
  if (!(root = json_parse_file(file))) ERROR("malformed package.json");
  if (!(obj = json_value_get_object(root))) ERROR("malformed package.json");

  require_string(name);
  require_string(version);
  // TODO: validate semver

  repo = json_object_get_string(obj, "repo");
  if (!repo) {
    WARN_MISSING("repo");
  } else {
    if (!(repo_name = parse_repo_name(repo))) WARN("invalid repo");
    if (!(repo_owner = parse_repo_owner(repo, NULL))) WARN("invalid repo");
  }

  require_string(description);
  require_string(license);

  if (!(src = json_object_get_array(obj, "src"))) {
    // if there are no sources, then you need an
    // install key.  otherwise, there's no point
    // in your lib.
    require_string(install);
  }

  if (!(keywords = json_object_get_array(obj, "keywords"))) {
    WARN_MISSING("keywords");
  }

done:
  printf("\n");
  if (root) json_value_free(root);
  free(directory);
  free(file);
  return rc;
}
