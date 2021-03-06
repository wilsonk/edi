#ifndef ELM_CODE_COMMON_H_
# define ELM_CODE_COMMON_H_

typedef struct _Elm_Code Elm_Code;
typedef struct _Elm_Code_File Elm_Code_File;

EAPI extern const Eo_Event_Description ELM_CODE_EVENT_LINE_SET_DONE;
EAPI extern const Eo_Event_Description ELM_CODE_EVENT_FILE_LOAD_DONE;

typedef enum {
   ELM_CODE_STATUS_TYPE_DEFAULT = 0,
   ELM_CODE_STATUS_TYPE_ERROR,

   ELM_CODE_STATUS_TYPE_ADDED,
   ELM_CODE_STATUS_TYPE_REMOVED,
   ELM_CODE_STATUS_TYPE_CHANGED,

   ELM_CODE_STATUS_TYPE_PASSED,
   ELM_CODE_STATUS_TYPE_FAILED,

   ELM_CODE_STATUS_TYPE_COUNT
} Elm_Code_Status_Type;


typedef enum {
   ELM_CODE_TOKEN_TYPE_DEFAULT = ELM_CODE_STATUS_TYPE_COUNT,
   ELM_CODE_TOKEN_TYPE_COMMENT,


   ELM_CODE_TOKEN_TYPE_ADDED,
   ELM_CODE_TOKEN_TYPE_REMOVED,
   ELM_CODE_TOKEN_TYPE_CHANGED,

   ELM_CODE_TOKEN_TYPE_CURSOR, // a pseudo type used for styling but may not be set on a cell
   ELM_CODE_TOKEN_TYPE_COUNT
} Elm_Code_Token_Type;

#ifdef __cplusplus
extern "C" {
#endif



/**
 * @file
 * @brief Common data structures and constants.
 */

struct _Elm_Code
{
   Elm_Code_File *file;
   Eina_List *widgets;
   Eina_List *parsers;
};

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* ELM_CODE_COMMON_H_ */
