/* t-sign.c  - regression test
 *	Copyright (C) 2000 Werner Koch (dd9jn)
 *      Copyright (C) 2001 g10 Code GmbH
 *
 * This file is part of GPGME.
 *
 * GPGME is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * GPGME is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <gpgme.h>

#define fail_if_err(a) do { if(a) {                                       \
                               fprintf (stderr, "%s:%d: GpgmeError %s\n", \
                                __FILE__, __LINE__, gpgme_strerror(a));   \
                                exit (1); }                               \
                             } while(0)

static void
print_op_info (GpgmeCtx c)
{
    char *s = gpgme_get_op_info (c, 0);

    if (!s)
        puts ("<!-- no operation info available -->");
    else {
        puts (s);
        free (s);
    }
}

static void
print_data ( GpgmeData dh )
{
    char buf[100];
    size_t nread;
    GpgmeError err;

    err = gpgme_data_rewind ( dh );
    fail_if_err (err);
    while ( !(err = gpgme_data_read ( dh, buf, 100, &nread )) ) {
        fwrite ( buf, nread, 1, stdout );
    }
    if (err != GPGME_EOF) 
        fail_if_err (err);
}

int 
main (int argc, char **argv )
{
    GpgmeCtx ctx;
    GpgmeError err;
    GpgmeData in, out;

  do {
    err = gpgme_new (&ctx);
    fail_if_err (err);
    gpgme_set_protocol (ctx, GPGME_PROTOCOL_CMS);
    gpgme_set_textmode (ctx, 1);
    gpgme_set_armor (ctx, 1);

    err = gpgme_data_new_from_mem ( &in, "Hallo Leute!\n", 13, 0 );
    fail_if_err (err);

#if 0
    /* first a normal signature */
    err = gpgme_data_new ( &out );
    fail_if_err (err);
    err = gpgme_op_sign (ctx, in, out, GPGME_SIG_MODE_NORMAL );
    fail_if_err (err);
    fflush (NULL);
    fputs ("Begin Result:\n", stdout );
    print_op_info (ctx);
    print_data (out);
    fputs ("End Result.\n", stdout );
    gpgme_data_release (out);
    gpgme_data_rewind (in);
#endif
    
    /* now a detached signature */
    err = gpgme_data_new ( &out );
    fail_if_err (err);
    err = gpgme_op_sign (ctx, in, out, GPGME_SIG_MODE_DETACH );
    fail_if_err (err);
    fflush (NULL);
    print_op_info (ctx);
    fputs ("Begin Result:\n", stdout );
    print_data (out);
    fputs ("End Result.\n", stdout );
    gpgme_data_release (out);
    gpgme_data_rewind (in);
    

#if 0
    /* And finally a cleartext signature */
    err = gpgme_data_new ( &out );
    fail_if_err (err);
    err = gpgme_op_sign (ctx, in, out, GPGME_SIG_MODE_CLEAR );
    fail_if_err (err);
    fflush (NULL);
    print_op_info (ctx);
    fputs ("Begin Result:\n", stdout );
    print_data (out);
    fputs ("End Result.\n", stdout );
    gpgme_data_release (out);
    gpgme_data_rewind (in);
#endif
    
    /* ready */
    gpgme_data_release (in);
    gpgme_release (ctx);
  } while ( argc > 1 && !strcmp( argv[1], "--loop" ) );
   
    return 0;
}



