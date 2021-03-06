#ifdef HAVE_CONFIG
# include "config.h"
#endif

#include "Elm_Code.h"

#include "elm_code_private.h"

static Elm_Code_Line *_elm_code_blank_create(int line)
{
   Elm_Code_Line *ecl;

   ecl = calloc(1, sizeof(Elm_Code_Line));
   if (!ecl) return NULL;

   ecl->number = line;
   ecl->status = ELM_CODE_STATUS_TYPE_DEFAULT;
   return ecl;
}

static void _elm_code_file_line_append_data(Elm_Code_File *file, const char *content, int length, int row, Eina_Bool mapped)
{
   Elm_Code_Line *line;

   line = _elm_code_blank_create(row);
   if (!line) return;

   if (mapped)
     {
        line->content = content;
        line->length = length;
     }
   else
     {
        line->modified = malloc(sizeof(char)*(length+1));
        strncpy(line->modified, content, length);
        line->modified[length] = 0;
        line->length = length;
     }

   file->lines = eina_list_append(file->lines, line);

   if (file->parent)
     {
        elm_code_parse_line(file->parent, line);
        elm_code_callback_fire(file->parent, &ELM_CODE_EVENT_LINE_SET_DONE, line);
     }
}

EAPI Elm_Code_File *elm_code_file_new(Elm_Code *code)
{
   Elm_Code_File *ret;

   if (code->file)
     elm_code_file_free(code->file);

   ret = calloc(1, sizeof(Elm_Code_File));
   code->file = ret;
   ret->parent = code;

   return ret;
}

EAPI Elm_Code_File *elm_code_file_open(Elm_Code *code, const char *path)
{
   Elm_Code_File *ret;
   Eina_File *file;
   Eina_File_Line *line;
   Eina_Iterator *it;
   unsigned int lastindex;

   ret = elm_code_file_new(code);
   file = eina_file_open(path, EINA_FALSE);
   ret->file = file;
   lastindex = 1;

   ret->map = eina_file_map_all(file, EINA_FILE_POPULATE);
   it = eina_file_map_lines(file);
   EINA_ITERATOR_FOREACH(it, line)
     {
        Elm_Code_Line *ecl;

        /* Working around the issue that eina_file_map_lines does not trigger an item for empty lines */
        while (lastindex < line->index - 1)
          {
             ecl = _elm_code_blank_create(++lastindex);
             if (!ecl) continue;

             ret->lines = eina_list_append(ret->lines, ecl);
          }

        _elm_code_file_line_append_data(ret, line->start, line->length, lastindex = line->index, EINA_TRUE);
     }
   eina_iterator_free(it);

   if (ret->parent)
     {
        elm_code_parse_file(ret->parent, ret);
        elm_code_callback_fire(ret->parent, &ELM_CODE_EVENT_FILE_LOAD_DONE, ret);
     }
   return ret;
}

EAPI void elm_code_file_free(Elm_Code_File *file)
{
   Elm_Code_Line *l;

   EINA_LIST_FREE(file->lines, l)
     {
        if (l->modified)
          free(l->modified);
        free(l);
     }

   if (file->file)
     {
        if (file->map)
          eina_file_map_free(file->file, file->map);

        eina_file_close(file->file);
     }
   free(file);
}

EAPI void elm_code_file_close(Elm_Code_File *file)
{
   eina_file_close(file->file);
}

EAPI const char *elm_code_file_filename_get(Elm_Code_File *file)
{
   return basename((char *)eina_file_filename_get(file->file));
}

EAPI const char *elm_code_file_path_get(Elm_Code_File *file)
{
   return eina_file_filename_get(file->file);
}

EAPI void elm_code_file_clear(Elm_Code_File *file)
{
   Elm_Code_Line *l;

   EINA_LIST_FREE(file->lines, l)
     {
        if (l->modified)
          free(l->modified);

        free(l);
     }

   if (file->parent)
     elm_code_callback_fire(file->parent, &ELM_CODE_EVENT_FILE_LOAD_DONE, file);
}

EAPI unsigned int elm_code_file_lines_get(Elm_Code_File *file)
{
   return eina_list_count(file->lines);
}


EAPI void elm_code_file_line_append(Elm_Code_File *file, const char *line, int length)
{
   int row;

   row = elm_code_file_lines_get(file);
   _elm_code_file_line_append_data(file, line, length, row+1, EINA_FALSE);
}

EAPI Elm_Code_Line *elm_code_file_line_get(Elm_Code_File *file, unsigned int number)
{
   return eina_list_nth(file->lines, number - 1);
}

EAPI const char *elm_code_file_line_content_get(Elm_Code_File *file, unsigned int number, int *length)
{
   Elm_Code_Line *line;

   line = elm_code_file_line_get(file, number);

   if (!line)
     return NULL;

   *length = line->length;

   if (line->modified)
     return line->modified;
   return line->content;
}

EAPI void elm_code_file_line_status_set(Elm_Code_File *file, unsigned int number, Elm_Code_Status_Type status)
{
   Elm_Code_Line *line;

   line = elm_code_file_line_get(file, number);

   if (!line)
     return;

   line->status = status;

   if (file->parent)
     elm_code_callback_fire(file->parent, &ELM_CODE_EVENT_LINE_SET_DONE, line);
}

EAPI void elm_code_file_line_token_add(Elm_Code_File *file, unsigned int number, int start, int end,
                                       Elm_Code_Token_Type type)
{
   Elm_Code_Line *line;
   Elm_Code_Token *tok;

   line = elm_code_file_line_get(file, number);
   tok = calloc(1, sizeof(Elm_Code_Token));

   tok->start = start;
   tok->end = end;
   tok->type = type;

   line->tokens = eina_list_append(line->tokens, tok);

   if (file->parent)
     elm_code_callback_fire(file->parent, &ELM_CODE_EVENT_LINE_SET_DONE, line);
}

