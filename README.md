# Instant messaging bridge

An instant messaging bridge across some platforms.

# Protocol

All messages are internally distributed via a certain protocol.
This protocol is transmitted via a unix-socket.

The sockets are located in a directory specified in the config file or
default in `./sockets/` relative to the core module and destroyed on program
start and end. The full socket path is passed on startup to every module using the
-s parameter. As protocol TCP is used, in order to allow a continuous flow of data.

A typical protocol message to and from the server is structured like this:

`<type>\u001f<from>\u001f<message>\u0004`

`\u0004` --> Terminator char.

`\u001f` --> (INFORMATION SEPARATOR ONE) The separator between each bit of information.

`<type>` --> The message type as an unsigned int. eg 0 --> command etc.

`<from>` --> The user who sent the message.

`<message>` --> The received message to be interpreted by the core module.

The Terminator char has to be filtered out of messages by the modules. I used
TCP to guarantee a continuous flow of data, and the unicode characters because they
were the most obvious ones. I mean information seperator one and end of transmission…
Message types are sent as an integer. The core has an enum for them, but there is no
specification for the core modules to do the same.

The message types are:

0 --> Configuration   e.g. the to be connected server

1 --> User command    e.g. a ban or kick

2 --> System message  e.g. a broadcast message

3 --> User message    e.g. "Hello" or "How are you"

4 --> Breach          e.g. a f*cking idiot who tried to missuse the protocol.


The message structure may vary between each message type.
For further details please refere to either the source code or a documentation.

TODO: Documentation

If a 4/Breach is received, the module may shut down, due to the security hazard, the
core will now reset all it's connections and restart itself.

# Configuration

The configuration is done via a single json config file. All options are then
transmitted internally.

If the config file is not specified via the command line argument -c or --config
the core module will look for it in the following order:

 - `./config.json`
 - `/etc/cross-msg/config.json`
 - `~/.config/cross-msg/config.json`

If none is found, the module will exit.

An configuration file can consist of several module blocks which store specific
information on each and every one of them. e.g:

`{

  "socketdir" : "<Directory to save sockets>",

  "modules"   : [

    {"title"   : "<Module name>",

     "socket"  : "<Socket name>",

     "command" : "<the start command for the module>",

     "config"  : {"server" : "irc.freenode.net", "port" : "6667"} },

     {"title"  : "<Module name>",

     "socket"  : "<Socket name>",

     "command" : "<the start command for the module>",

     "config"  : { "server" : "irc.furnet.net", "port" : "6667" } }

  ]

}`

The config block in each module block is transmitted argument for argument
on startup like this:

`0\u001fserver\u001firc.freenode.net\u0004`

the according line in the config array would be:

`"server" : "irc.freenode.net"`

This way of configuring means a much easier way of implementation for the programmer,
but due to this strange way of configuration, the user has to think about the config
before he is able to write it properly. I expect bug reports. Many bug reports.
