# FontChecker
Did you put many fonts to your just installed Windows 10 and got some programs with strange symbols instead of normal characters? This program can help you to solve this problem.

### Problem
As described [here](http://answers.microsoft.com/en-us/windows/forum/windows_10-start/wrong-graffiti-font-in-apps-and-web-pages-after/eb9f39c0-989a-41a0-91d6-8b8ac44cc298) Windows 10 has an issue with some fonts. There are some fonts that has wrong strings in theirs metadata - they have different family names and identifiers for different platforms. For example, one font can have name "Jungle LIFE" for Windows and for some reasons it pretends to be Arial on Mac.
Although previous versions of Windows works fine with it, Windows 10 replaces system fonts by these third-party fonts. As a result some programs (e.g. Steam) shows "graffiti" instead of normal characters (Steam has checkbox to disable DirectWrite but others hasn't).

### Solution
Actually there are two similar console programs.

**FontChecker** takes the font file (.ttf, .ttc and .otf are supported) and compares Font Family and Unique Identifier against same ones for other platforms presented in the names table of font file. It guesses font is wrong if there are differences between these strings.

**StringExporter** exports all strings from names table of font to the human-readable text file.

### Usage
To perform check execute ```FontChecker.exe "path_to_your_directory_with_fonts"``` or just drag'n'drop directory with fonts to FontChecker.exe file. It will show green valid fonts and reds fonts with suspicious tags. So now you know the ones that cause the problems. Just go to the ContolPanel/Fonts and uninstall them.

However, some fonts don't make problems but simply has errors in their metadata that causes false-positive result in FontChecker. You can use **StringExporter** to manually check that certain font has problems or not. Or don't bother yourself more and uninstall it too.

Execute ```StringExporter.exe "font_file1" "font_file2" ...``` or just select font files and drag'n'drop them on StringExporter.exe. Text files with same name as font files will appear near. Data will be shown in the following format:

```
(x,y,z): n - string
...
```

where ```x``` - platform id, ```y``` - encoding id, ```z``` - language id, ```n``` - name id. **FontChecker** compares only strings with n == 1 (font family) and n == 3 (unique identifier). For more information check list of ids and their meanings in the [specification](https://www.microsoft.com/typography/otspec/name.htm).

The project compiles with Visual Studio 2015. You can also get binaries [here](https://github.com/BlackOverlord666/FontChecker/releases).
