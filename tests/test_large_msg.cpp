/*
    Copyright (c) 2007-2015 Contributors as noted in the AUTHORS file

    This file is part of libzmq, the ZeroMQ core engine in C++.

    libzmq is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    As a special exception, the Contributors give you permission to link
    this library with independent modules to produce an executable,
    regardless of the license terms of these independent modules, and to
    copy and distribute the resulting executable under terms of your choice,
    provided that you also meet, for each linked independent module, the
    terms and conditions of the license of that module. An independent
    module is a module which is not derived from or based on this library.
    If you modify this library, you must extend this exception to your
    version of the library.

    libzmq is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
    License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <limits.h>
#include "testutil.hpp"

int main (void)
{
    setup_test_environment();
    void *ctx = zmq_ctx_new ();
    assert (ctx);

    void *sb = zmq_socket (ctx, ZMQ_PAIR);
    assert (sb);
    int rc = zmq_bind (sb, "tcp://127.0.0.1:5560");
    assert (rc == 0);

    void *sc = zmq_socket (ctx, ZMQ_PAIR);
    assert (sc);
    rc = zmq_connect (sc, "tcp://127.0.0.1:5560");
    assert (rc == 0);
    
    zmq_msg_t msg, rcv;
    size_t big = 64 + (size_t) INT_MAX;
    printf("Large msg: %u %llu %lu\n", INT_MAX, SIZE_MAX, big);
    
    rc = zmq_msg_init_size (&msg, big);
    assert (rc == 0);
    size_t sz = zmq_msg_size(&msg);
    assert (sz == big);
    
    rc = zmq_msg_send(&msg, sb, 0);
    assert (rc >= 0);       // first check success
    assert (rc == INT_MAX); // check truncated max return value
    
    rc = zmq_msg_close(&msg);
    assert (rc == 0);
    
    rc = zmq_msg_init(&rcv);
    assert (rc == 0);
    
    rc = zmq_msg_recv(&rcv, sc, 0);
    assert (rc >= 0);       // first check success
    assert (rc == INT_MAX); // check truncated max return value
    
    sz = zmq_msg_size(&rcv);
    assert (sz == big); // check message is still the right size
    
    rc = zmq_msg_close(&rcv);
    assert (rc == 0);
    
    rc = zmq_close (sc);
    assert (rc == 0);

    rc = zmq_close (sb);
    assert (rc == 0);

    rc = zmq_ctx_term (ctx);
    assert (rc == 0);

    return 0 ;
}
