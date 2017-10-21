# Instant messaging bridge

An instand messaging bridge across some platforms

# Protocol

All messages are internally distributed via a certain protocol.
This protocol is transmitted via a unix-socket.

A typical message to and from the server is structured like this:

´<type>\u001f<from/to>\u001f<message>\u0004´

´\u0004´ --> Terminator char.
´\u001f´ --> (INFORMATION SEPARATOR ONE) The separator between each bit of information
´<type>´ --> The message type as an int. eg 0 --> command etc.
´<form/to>´ --> The module to send the informationto/ where it comes from as it's name.
´<message>´ --> The received message to be interpreted by the core module. 
