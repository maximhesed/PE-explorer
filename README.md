I relied on this article: http://www.csn.ul.ie/~caolan/pub/winresdump/winresdump/doc/pefile2.html
for understanding a PE file format structure. I didn't read everything yet, but I thought, that
would be interesting to write an own small system, to display a various data from
the PE files. I understand, that some text file would copes with this task.
Nonetheless, on that moment, I already was keen on it.

In essentially, this system is just a set of the spoilers (the spoiler is one of the GUI widgets).
I partially implemented the spoiler for the console windows (in Windows). It accepts a data
and display it through a buffer. So, you can drag a PE file (.dll, .exe) onto a
console screen or specify its path in the arguments, to get an information about this file.

Only the non-loaded DLLs are supported.

