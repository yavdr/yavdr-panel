#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <glib.h>
#include <gtk/gtk.h>
#define GMENU_I_KNOW_THIS_IS_UNSTABLE
#include <gmenu-tree.h>

char *filter(const char *str)
{
  GError *error = NULL;
  GRegex *regex;
  char *result;

  regex = g_regex_new(" [^ ]*%[fFuUdDnNickvm]", 0, 0, &error);
  result = g_regex_replace_literal(regex, str, strlen(str), 0, "", 0, &error);
  g_regex_unref(regex);
 
  return result;
}

char *escape(const char *str)
{
  const char *pstr = str;
  char *buf = malloc(strlen(str) * 6 + 1); 
  char *pbuf = buf;

  struct {
    char *from;
    char *to;
  } tab[] = {
    {"ä", "&#xE4;"},
    {"ö", "&#xF6;"},
    {"ü", "&#xFC;"},
    {"Ä", "&#xC4;"},
    {"Ö", "&#xD6;"},
    {"Ü", "&#xDC;"},
    {"ß", "&#xDF;"},
    {"<", "&#x3C;"},
    {">", "&#x3E;"},
  };

  while (*pstr)
  {
    int i;
    int found = 0;

    for (i = 0; i < sizeof(tab) / sizeof(tab[0]); i++)
    {
      if (!strncmp(pstr, tab[i].from, 2))
      {
        strcpy(pbuf, tab[i].to);
        pbuf += strlen(tab[i].to);
        pstr++;
        found = 1;
        break;
      }
    }
    if (!found) 
    {
      *pbuf++ = *pstr;
    }
    pstr++;
  }
  *pbuf = '\0';
  return buf;
}

void walk_tree(GMenuTreeDirectory *root)
{
  GSList *list = gmenu_tree_directory_get_contents(root);

  while (list)
  {
    const char *category;
    const char *application;
    const char *exec;

    switch (gmenu_tree_item_get_type (list->data))
    {
      case GMENU_TREE_ITEM_DIRECTORY:
        category = escape(gmenu_tree_directory_get_name (list->data));
        if (category)
        {
          printf("<menu name=\"%s\">\n", category);
          free((void *)category);
        }
        
        GMenuTreeDirectory *dir = (GMenuTreeDirectory*)list->data;
        walk_tree(dir);
        printf("</menu>\n");
        break;
      case GMENU_TREE_ITEM_ENTRY:
        if (!gmenu_tree_entry_get_is_excluded(list->data) &&
            !gmenu_tree_entry_get_is_nodisplay(list->data))
        {
          application = escape(gmenu_tree_entry_get_name(list->data));
          if (application)
          {
            exec = filter(gmenu_tree_entry_get_exec(list->data));
            if (exec)
            {
              if (gmenu_tree_entry_get_launch_in_terminal(list->data))
              {
                printf("<command name=\"%s\" execute=\"xterm -title '%s' -e '%s' &amp;\" \\>\n", application, application, exec);
              }
              else
              {
                printf("<command name=\"%s\" execute=\"%s &amp;\" \\>\n", application, exec);
              }
              free((void *)application);
            }
          }
        }
        break;
      default:
        break;
    }
    list = g_slist_next(list);
  }
}

void main(int argc, char *argv[])
{
  GMenuTree *tree = NULL;
  GMenuTreeDirectory *directory = NULL;
  GMenuTreeEntry *entry = NULL;
  const char *name = NULL;

  gtk_set_locale ();

  tree = gmenu_tree_lookup ("lxde-applications.menu", GMENU_TREE_FLAGS_NONE);
  directory = gmenu_tree_get_root_directory(tree);

  walk_tree(directory);
}
