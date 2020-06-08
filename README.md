I relied on this article: https://shorturl.at/vBUWY for understanding a PE 
file format structure. I didn't read everything, yet, but I thought, that 
would interesting write an own small system for display a various data from 
PE files. Ya, I understand, that the some text file would copes with this task, 
but, on that moment, I already was keen on it. (Ah, its marvelous moments, 
when you writing a code... Writing at day, writing at night... 
Just writing and have delight.)

In essentially, this system is just a set of spoilers (one of GUI widgets). 
I partially implemented the spoiler for console windows (in Windows). It accepts a data 
and display it through the buffer. So, you can drag a PE file (.dll, .exe) onto a 
console screen or specify its path in arguments to get an information about it.

Only non-loaded DLLs are supported.

