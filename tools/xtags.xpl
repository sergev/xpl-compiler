/*

	Create a tags file for XPL programs.
	The tags file is used by VI to quickly find procedures and
	definitions.

	Written by Daniel Weaver
        May 5 2020

*/

declare TRUE literally '1',
    FALSE literally '0',
    WORD literally 'fixed',
    FOREVER literally 'while 1';

/* commonly used strings */
declare x1 character initial(' '),
    comma character initial(',');

/*                   **** declarations for the scanner ****           */

/*
      cp is the pointer to the last character scanned in the cardimage,
      text_limit length of text minus one */
declare cp WORD,
    text_limit WORD;

declare idt(255) bit(1);  /* Identifier type 1->Starter, 2->digit */

/* Lower to upper case conversion */
declare upper(255) bit(8);

/* alphabet consists of the symbols considered alphabetic in building
      identifiers.  These characters may start an identifier.  */
declare alphabet character initial
    ('abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_$@#');

declare text character,
    src fixed,  /* Source file unit number */
    TAB literally 'byte("(c)\t")',
    quote WORD,  /* Count each single quote */
    white(255) bit(1),  /* TRUE if blank or TAB */
    IDENTIFIER_TOKEN literally '256',
    (ids, idl) WORD;  /* <identifier> start and length */

/* Characters that must be escaped within tags */
declare meta_characters character initial('/\$^'),
    special(255) bit(8);

/* Tag Table */
declare NUM_TAGS literally '511',
    tags WORD,  /* Number of tags in the table */
    tag_start(NUM_TAGS) bit(8),  /* Index of the first character of the tag */
    tag_min(NUM_TAGS) bit(8),  /* Minimum position for search string */
    tag_length(NUM_TAGS) bit(8),  /* Length of the tag */
    tag_file(NUM_TAGS) bit(8),  /* Index of filename in the argv array */
    tag_line(NUM_TAGS) character,  /* The line that contains the tag */
    tag_sort(NUM_TAGS) bit(16);  /* Used to sort the tags */

/*
**      Compare two strings.  Ignore case.
**
**      if s > t returns positive number;
**      if s < t returns negative number;
**      if s = t returns zero;
*/
case_compare:
procedure(s, t) fixed;
    declare (s, t) character;
    declare (i, a, b) fixed;

    a = length(s);
    b = length(t);
    if a ~= b then return a - b;
    do i = 0 to length(s) - 1;
        a = upper(byte(s, i));
        b = upper(byte(t, i));
        if a ~= b then return a - b;
    end;
    return 0;
end case_compare;

/*
**      Compare two strings.
**
**      if s > t returns positive number;
**      if s < t returns negative number;
**      if s = t returns zero;
*/
compare:
procedure(s, t) fixed;
    declare (s, t) character;
    declare (i, a, b, len, lena, lenb) fixed;

    lena = length(s);
    lenb = length(t);
    if lena > lenb then len = lenb;
    else len = lena;
    do i = 0 to len - 1;
        a = byte(s, i);
        b = byte(t, i);
        if a ~= b then return a - b;
    end;
    if lena = lenb then return 0;
    return lena - lenb;
end compare;


/*
**    scan()
*/
scan:
procedure WORD;
    declare c WORD;

    do while white(byte(text, cp));
        cp = cp + 1;
        if cp > text_limit then return 0;
    end;
    c = byte(text, cp);
    if idt(c) = 1 then
        do;
            ids = cp;
            cp = cp + 1;
            do while cp < text_limit;
                if idt(byte(text, cp)) = 0 then
                    do;
                        idl = cp - ids;
                        return IDENTIFIER_TOKEN;
                    end;
                cp = cp + 1;
            end;
            idl = cp - ids;
            return IDENTIFIER_TOKEN;
        end;
    cp = cp + 1;
    return c;
end scan;

/*
**   enter(filenumber, start, length)
**
**   Enter a new tag into the TAG table.
*/
enter:
procedure(f, s, l);
    declare (f, s, l) WORD;

    if (quote & 1) = 1 then return;
    if tags <= NUM_TAGS then
        do;
            tag_start(tags) = s;
            tag_min(tags) = cp;
            tag_length(tags) = l;
            tag_file(tags) = f;
            tag_line(tags) = text;
            tags = tags + 1;
        end;
end enter;

/*
**    read_file()
**
**    Read one source file and find the tags in that file.
*/
read_file:
procedure(f) WORD;
    declare (f, s, len, token) WORD;

    src = xfopen(argv(f), 'r');
    if src < 0 then
        do;
            output = 'Open file error: ' || argv(f);
            return 1;
        end;
    len = 0;
    text = input(src);
    do while length(text) > 0;
        quote, len, cp = 0;
        text_limit = length(text) - 1;
        do while cp <= text_limit;
            token = scan;
            if token = byte('''') then quote = quote + 1;
            else
            if token = byte(':') then
                do;
                    /* Look for <identifier> : */
                    if len > 0 then call enter(f, s, len);
                    len = 0;
                end;
            else
            if token ^= IDENTIFIER_TOKEN then len = 0;
            else
            if len = 0 then
                do;
                    s = ids;
                    len = idl;
                end;
            else
            if case_compare(substr(text, ids, idl), 'LITERALLY') = 0 then
                do;
                    /* Found <identifier> LITERALLY */
                    call enter(f, s, len);
                    len = 0;
                end;
            else
                do;
                    s = ids;
                    len = idl;
                end;
        end;
        text = input(src);
    end;
    call xfclose(src);
    return 0;
end read_file;

/*
**    sort_tags()
**
**    Sort the tags table.
*/
sort_tags:
procedure;
    declare (i, j, k, limit) WORD;
    declare (a, b) character;

    do i = 0 to tags - 1;
        tag_sort(i) = i;
    end;
    limit = tags - 1;
    do i = 0 to limit;
        k = tag_sort(i);
        a = substr(tag_line(k), tag_start(k), tag_length(k));
        do j = i + 1 to limit;
            k = tag_sort(j);
            b = substr(tag_line(k), tag_start(k), tag_length(k));
            if compare(a, b) > 0 then
                do;
                    k = tag_sort(i);
                    tag_sort(i) = tag_sort(j);
                    tag_sort(j) = k;
                    a = b;
                end;
        end;
    end;
end sort_tags;

/*
**   search_string(t)
**
**   Create the search string to find the tag at t.
*/
search_string:
procedure(t) character;
    declare (t, i) WORD;
    declare (head, text, tail, s) character;

    text = tag_line(t);
    if tag_start(t) = 0 then head = '/^';
    else
        do;
            head = '/';
            text = substr(text, tag_start(t));
        end;
    if length(text) <= 50 then tail = '$/';
    else
        do;
            tail = '/';
            text = substr(text, 0, 50);
        end;
    s = head;
    do i = 0 to length(text) - 1;
        if special(byte(text, i)) then s = s || '\';
        s = s || substr(text, i, 1);
    end;
    return s || tail;
end search_string;

/*
**   write_tags_file
**
**   Write the tags file.
*/
write_tags_file:
procedure WORD;
    declare (i, j, unit) WORD;
    declare (name, new, line) character;

    call sort_tags;
    name = '';
    unit = xfopen('tags', 'w');
    if unit < 0 then
        do;
            output = 'Open file error: tags';
            return 1;
        end;
    do i = 0 to tags - 1;
        j = tag_sort(i);
        new = substr(tag_line(j), tag_start(j), tag_length(j));
        if name = new then
            output = 'Duplicate definition for: ' || name || ' ignored';
        else
            do;
                line = search_string(j);
                output(unit) = new || "(c)\t" || argv(tag_file(j))
                    || "(c)\t" || line;
            end;
        name = new;
    end;
    call xfclose(unit);
    return 0;
end write_tags_file;

initialization:
procedure;
    /* initialize scanner tables and global variables */
    declare (i, j) fixed;

    do i = 0 to 255;
        upper(i) = i;
    end;
    do i = 0 to length(alphabet) - 1;
        j = byte(alphabet, i);
        idt(j) = 1;
        if i < 26 then upper(j) = byte(alphabet, i + 26);
    end;
    do i = byte('0') to byte('9');
        idt(i) = 2;
    end;
    do i = 0 to length(meta_characters) - 1;
        j = byte(meta_characters, i);
        special(j) = 1;
    end;
    white(byte(' ')) = 1;
    white(TAB) = 1;
end initialization;

usage:
procedure;
    output = 'Usage: ' || argv(0) || ' { <source-file> } ...';
    output = '   <source-file>   One or more XPL source files';
    output = '   --help          Print this help message';
end usage;

gather_info:
procedure WORD;
    declare i fixed;

    do i = 1 to argc - 1;
        if argv(i) = '--help' then
            do;
                call usage;
                return 1;
            end;
        if read_file(i) then return 1;
    end;
    return 0;
end gather_info;

call initialization;

if gather_info then return 1;
if write_tags_file then return 1;

return 0;

eof;
