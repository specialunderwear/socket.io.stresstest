Stresstest client for socket.io
===============================

Usage:

    ./manyclients.sh [-v] [-w seconds] 100 http(s)://host/ inputfile

``-v`` will show you the received responses.

``-w`` (seconds) will spawn a new process only every (seconds) seconds, instead of all at once

Your inputfile should be a json list with the exact requests the browser would send to your backend:

    [
        "5:::{\"name\":\"login\",\"args\":[{\"user\":\"michaeljackson\",\"password\":\"beatit\"}]}",
        "5:::{\"name\":\"poke\",\"args\":[{\"target\":\"prince\",\"sessionid\":\"%1%\"}]}",
        "0::"
    ]

the stresstest will execute all these requests in that order and ignore the
responses, except in one special case. You can make the client click through
your site this way. Always end with ``0::``.

Notice the ``%1%`` in the second request? That will be replaced with the sessionid
returned in the first request. Modify ``_parse_session_id`` inside
SocketIOHandler.cpp to parse it correctly from you login response.
