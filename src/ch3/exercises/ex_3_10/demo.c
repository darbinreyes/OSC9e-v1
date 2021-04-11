/*

  On Mac OS X

  To compile: make demo
  To run: ./demo -K dot  > out.dot && open out.dot

  The resulting out.dot file can be opened using the Graphviz GUI App.

  The resulting graph is huge, the Graphviz GUI makes it much easier to view.

*/

/* $Id$ $Revision$ */
/* vim:set shiftwidth=4 ts=8: */

/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: See CVS logs. Details at http://www.graphviz.org/
 *************************************************************************/

/* Note that, with the call to gvParseArgs(), this application assumes that
 * a known layout algorithm is going to be specified. This can be done either
 * using argv[0] or requiring the user to run this code with a -K flag specifying
 * which layout to use. In the former case, after this program has been built as
 * 'demo', you will need to rename it as one of the installed layout engines such
 * as dot, neato, sfdp, etc.
 */

#include <gvc.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define MAX_PID_STR_SIZE 64
#define MAX_LINE_SIZE    4000
#define NAME_PREFIX_STR "pid = "
#define GV_CREATE_FLAG 1
#define GV_NO_CREATE_FLAG 0
#define GV_EDGE_NO_NAME NULL

/*

  Returns a pointer to the last name in cmd.

  e.g.

  /usr/sbin/syslogd -> syslogd.

  autofsd -> autofsd.

  @param[in] cmd  Pointer to a string containing a path to an executable.
                  This corresponds to the CMD column of the "ps -ael" command.

  @return A pointer to the last name in the path.

*/
char *cmd_last_name(char *cmd) { // TODO use const qualifiers.
  char *c;
  assert(cmd != NULL);

  c = strrchr(cmd, '/');

  if (c == NULL) {
    // No '/' in cmd, return it as is.
    return cmd;
  }

  return c + 1; // + 1 advances past the '/' char.
}

/*

  Creates a node name. The name format is e.g. "pid = 123".

  @param[in,out] name  The buffer in which the name will be created.
  @param[in]     pid   The pid string.

*/
void mknodename(char *name, char *pid) {
  char *s;

  assert(name != NULL && pid != NULL);

  name[0] = '\0';

  s = strcat(name, NAME_PREFIX_STR);
  assert(s == name);

  s = strcat(name, pid);
  assert(s == name);
}

/*

  Sets the label for a node.
  The label format is e.g.:

  pid = 123
  launchd


  In Graphviz, it seems that a node's label is distinct from the node's name,
  however, the default label of a node is its name. This function fetches the
  current label, appends the cmd string on a new line, and sets the label to
  the resulting string.

  @param[in]      n      The node whose label is being set.
  @param[in]      cmd    The cmd string.

*/
void setnodelabel(Agnode_t *n, char *cmd) {
  char label[sizeof(NAME_PREFIX_STR) + MAX_PID_STR_SIZE + MAX_LINE_SIZE];
  char *s;
  assert(n != NULL && cmd != NULL);

  s = agget(n, "label");

  assert(s != NULL);

  label[0] = '\0'; // TODO use sprintf().
  strcat(label, s);
  strcat(label, "\n");
  strcat(label, cmd_last_name(cmd));
  agsafeset(n, "label", label, "");
}

int main(int argc, char **argv)
{
  // Graphviz vars.
  Agraph_t *g;
  Agnode_t *n, *m;
  Agedge_t *e;
  GVC_t *gvc;
  FILE *fp;

  // Vars for processing the input file.
  char line[MAX_LINE_SIZE];
  char pid[MAX_PID_STR_SIZE], ppid[MAX_PID_STR_SIZE];
  char pidlabel[sizeof(NAME_PREFIX_STR) + MAX_PID_STR_SIZE + MAX_LINE_SIZE];
  char cmd[MAX_LINE_SIZE];
  int nscan;
  char *s;
  int r;

  /* set up a graphviz context */
  gvc = gvContext();

  /* parse command line args - minimally argv[0] sets layout engine */
  gvParseArgs(gvc, argc, argv);

  /* Create a simple digraph */
  g = agopen("g", Agdirected, 0);
  assert(g != NULL);

  fp = fopen("ps_out.txt", "r");
  assert(fp != NULL);

  s = fgets(line, MAX_LINE_SIZE, fp); // Discard the first line.
  assert(s != NULL);

  // For each line of the input file, extract the PID, PPID, and CMD values.
  while ( (nscan = fscanf(fp, "%*d %s %s %*s %*s %*s %*s %*s %*s %*s %*s %*s \
                               %*s %*s %s\n", pid, ppid, cmd)) >= 0) {

    if (nscan == 0) {
      /*

      Discard the far end of a line after the executable name in the input
      file. e.g. if a line ends with "ps -ael", discard " -ael"

      */
      s = fgets(line, MAX_LINE_SIZE, fp);
      assert(s != NULL);
    } else {


      mknodename(pidlabel, pid);

      /*

        If the node doesn't exist, create it. Otherwise fetch
        the existing node. pid = process id.

      */
      if ( (n = agnode(g, pidlabel, GV_NO_CREATE_FLAG)) == NULL ) {
        n = agnode(g, pidlabel, GV_CREATE_FLAG);
        assert(n != NULL);
        setnodelabel(n, cmd);
      }

      mknodename(pidlabel, ppid);

      // Repeat above for name = ppid. ppid = parent process id.
      if ( (m = agnode(g, pidlabel, GV_NO_CREATE_FLAG)) == NULL ) {
        m = agnode(g, pidlabel, GV_CREATE_FLAG);
        assert(m != NULL);
      }

      // Add an edge from ppid (parent process id) to pid, if it doesn't exist
      // yet.
      if ( (e = agedge(g, m, n, GV_EDGE_NO_NAME, GV_NO_CREATE_FLAG)) == NULL) {
        e = agedge(g, m, n, GV_EDGE_NO_NAME, GV_CREATE_FLAG);
        assert(e != NULL);
        if(strcmp(ppid, "1") == 0) {
          // Set color = green for edges out of the "init" ("launchd" on Mac OS
          // X) process.
          agsafeset(e, "color", "green", "");
        }
      }
    }

  }

  r = fclose(fp);

  assert(r == 0);

  /* Compute a layout using layout engine from command line args */
  gvLayoutJobs(gvc, g);

  /* Write the graph according to -T and -o options */
  gvRenderJobs(gvc, g);

  /* Free layout data */
  gvFreeLayout(gvc, g);

  /* Free graph structures */
  agclose(g);

  /* close output file, free context, and return number of errors */
  return (gvFreeContext(gvc));
}

/* ******* C pitfall notes.

demo.c:76:15: warning: incompatible integer to pointer conversion assigning to
      'Agnode_t *' (aka 'struct Agnode_s *') from 'int' [-Wint-conversion]
      if ( (n = agidnode(g, pid, 1) == NULL) ) {
              ^ ~~~~~~~~~~~~~~~~~~~~~~~~~~~

I hit this WARNING and assumed it meant that, because n has type 'Agnode_t *', and
agidnode() returns type `int`, this line is performing a suspicious type conversion.

The man states that agidnode() returns type a 'Agnode_t *', so I assumed the man
page was incorrect. WRONG!

The return type is indeed 'Agnode_t *'.

The real problem was that I improperly parenthesized the assignment in the
"if"'s guard. This resulted in the expression n = agidnode() == NULL, which is
equivalent to (n = (agidnode() == NULL)), since "=="" binds tighter than "=",
and (agidnode() == NULL) produces an int value.

*/

/* ******* Graphviz pitfall.

  The id argument of agidnode() is NOT an int of your choosing. I gave up on
  figuring out how to use it. Search the man page for "agid" if you wish.

  Instead, use agnode() with name = int converted to a string.

*/

/* ******* From the man cgraph page - agopen.
Remark: The graphviz man pages are very low quality relative to the system man pages. The function prefix "a" probably means acyclic.


Agraph_t  *agopen(char *name, Agdesc_t kind, Agdisc_t *disc);

creates a new graph with the  given  name

arg0
  the  given  name

arg1
  Graph kinds
    are  Agdirected,  Agundirected, Agstrictdirected, and Agstric-
    tundirected.

arg2
  The  final argument
    points to a discpline structure
    to tailor I/O, memory allocation, and  ID  allocation.
      NULL
        default discipline AgDefault-Disc
*/

/* ******* From the man cgraph page - agnode.

Agnode_t  *agnode(Agraph_t *g, char *name, int createflag);
  agnode  searches  in  a  graph  or subgraph for a node with the given
  name, and returns it if found.

Agnode_t  *agidnode(Agraph_t *g, ulong id, int createflag);

*/

/* ******* From the man cgraph page - agedge.

Agedge_t  *agedge(Agraph_t* g, Agnode_t *t, Agnode_t *h, char *name, int createflag);

agedge searches in a graph or subgraph for an edge between the  given
     endpoints  (with an optional multi-edge selector name) and returns it
     if found or created.  Note that, in undirected graphs, a search tries
     both orderings of the tail and head nodes.  If the name is NULL, then
     an anonymous internal value is generated.

Agedge_t  *agidedge(Agraph_t * g, Agnode_t * t, Agnode_t * h, unsigned long id, int createflag);

*/
