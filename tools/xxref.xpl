/*
      XPL Cross reference program

      Department of Computer Science
      SUNY at Stony Brook

      This program was included in the Pascal compiler release dated 1976.
*/
/*
      Ported to the XPL to C translator by Daniel Weaver 2/2018.
         Added command line arguments and Usage display.
         Added upper and lower case keywords and identifiers.
         Added support variable length input and TABs.
         Added support for C++ style comments starting with double slash.
         Added table overflow handling.
*/
declare NT literally '42';
declare v(92) character initial('< dummy >', '<', '(', '+', '|',
    '&', '*', ')', ';', '~', '-', '/', ',', '>', ':', '=', '||',
    'by', 'do', 'go', 'if', 'to', 'bit', 'end', 'eof', 'mod', 'call',
    'case', 'else', 'goto', 'then', 'fixed', 'label', 'while',
    'return', 'declare', 'initial', '<number>', '<string>',
    'character', 'literally', 'procedure', '<identifier>', '<term>',
    '<type>', '<go to>', '<group>', '<ending>', '< start >',
    '<primary>', '<program>', '<replace>', '<bit head>',
    '<constant>', '<relation>', '<variable>', '<if clause>',
    '<left part>', '<statement>', '<true part>', '<assignment>',
    '<bound head>', '<expression>', '<group head>', '<if statement>',
    '<initial head>', '<initial list>', '<while clause>',
    '<case selector>', '<call statement>', '<logical factor>',
    '<parameter head>', '<parameter list>', '<procedure head>',
    '<procedure name>', '<statement list>', '<subscript head>',
    '<basic statement>', '<go to statement>', '<identifier list>',
    '<logical primary>', '<step definition>', '<label definition>',
    '<return statement>', '<type declaration>',
    '<iteration control>', '<logical secondary>',
    '<string expression>', '<declaration element>',
    '<procedure definition>', '<arithmetic expression>',
    '<declaration statement>', '<identifier specification>');
declare vt_index(13) bit(8) initial(0, 1, 16, 22, 26, 31, 34, 35,
    37, 39, 42, 42, 42, 43);

/*  Declarations for the scanner                                        */

/* token is the index into the vocabulary v() of the last symbol scanned,
   bcd is the last symbol scanned (literal character string). */
declare token fixed, bcd character;

/* Set up some convenient abbreviations for input/output control */
declare EJECT_PAGE literally 'output = page',
    DOUBLE_SPACE literally 'output = double',
    page character initial(''),
    double character initial(''),
    source_file fixed;

/* Length of longest symbol in v */
declare reserved_limit fixed;

/* chartype() is used to distinguish classes of symbols in the scanner.
   tx() is a table used for translating characters to token numbers.
   control() holds the value of the compiler control toggles set in $ cards.
   not_letter_or_digit() is similiar to chartype() but used in scanning
   identifiers only.

   All are used by the scanner and control() is set there.
*/
declare (chartype, tx)(255) bit(8),
    (control, not_letter_or_digit)(255) bit(1);

/* upper_case, lower_case and special_case define the characters that
   are valid in identifiers.
   upper_case and lower_case must be the same length.
 */
declare upper_case character initial('ABCDEFGHIJKLMNOPQRSTUVWXYZ');
declare lower_case character initial('abcdefghijklmnopqrstuvwxyz');
declare special_case character initial('_$@#');
declare lower(255) bit(8);   /* Map Upper Case characters to Lower Case */

/* buffer holds the latest cardimage,
   text holds the present state of the input text
   (not including the portions deleted by the scanner),
   cp is the pointer to the last character scanned in the cardimage,
   text_limit is a convenient place to store the pointer to the end of text,
   text_used is a count of characters already consumed on this cardimage,
   margin_chop holds the column number of sequence field,
   card_count is incremented by one for every source card read,
   error_count tabulates the errors as they are detected,
   previous_error holds the line number of the last error,
*/
declare (buffer, text) character,
    (cp, text_limit, text_used, margin_chop) fixed,
    (card_count, error_count, previous_error) fixed;
declare tokentype(NT) bit(16);

/* Each of the following contains the index into v() of the corresponding
   symbol.   We ask:    if token = ident    etc.    */
declare (ident, number, divide, string, orsymbol, catenate) fixed;

declare s character;  /* A temporary used various places */

declare end_message character initial('   ');

/* Commonly used strings */
declare x1 character initial(' '),
    x70 character initial(
    '                                                                      ');
declare quote character initial('''');
declare bitquote character initial('"');
declare stroke character initial('|');

declare TRUE literally '1', FALSE literally '0', FOREVER literally 'while 1';

declare NULL literally '-1';
declare MAXNEST literally '32';
declare SYTSIZE literally '1023';
declare sy_link(SYTSIZE) bit(16);
declare n_predeclared_symb fixed;
declare syt(SYTSIZE) character initial(
    'monitor_link', 'time_of_generation', 'date_of_generation',
    'coreword', 'corebyte', 'freepoint', 'descriptor', 'ndescript', 'length',
    'substr', 'byte', 'shl', 'shr', 'input', 'output', 'file', 'inline',
    'trace', 'untrace', 'exit', 'time', 'date', 'clock_trap', 'interrupt_trap',
    'monitor', 'addr', 'compactify');
declare (procedure_number, lex_level) fixed;
declare proc_stack(MAXNEST) bit(16);
declare (ndecsy, procmark) fixed;
declare bucket(255) bit(16);
declare (stackmark, endcount) (MAXNEST) bit(16);
declare LIST_SIZE literally '20000';
declare (ref_link, line_#) (LIST_SIZE) bit(16);
declare nextref fixed initial(0),
    (macrodef, macroend, not_found) bit(1),
    (ref_ptr, last_ref, owner) (SYTSIZE) bit(16);
declare compiling bit(1);


/*               P R O C E D U R E S                                  */

/*
**      Compare two strings.  Ignore case.
**
**      if a > b returns positive number;
**      if a < b returns negative number;
**      if a = b returns zero;
*/
case_compare:
procedure(s, t) fixed;
    declare (s, t) character;
    declare (i, a, b) fixed;

    a = length(s);
    b = length(t);
    if a ~= b then return a - b;
    do i = 0 to length(s) - 1;
        a = lower(byte(s, i));
        b = lower(byte(t, i));
        if a ~= b then return a - b;
    end;
    return 0;
end case_compare;

pad:
procedure(string, width) character;
    declare string character, (width, l) fixed;

    l = length(string);
    if l >= width then return string;
    do while width - l > length(x70);
        string = string || x70;
        l = l + length(x70);
    end;
    return string || substr(x70, 0, width - l);
end pad;

i_format:
procedure(number, width) character;
    declare (number, width, l) fixed, string character;

    string = number;
    l = length(string);
    if l >= width then return string;
    else return substr(x70, 0, width - l) || string;
end i_format;

error:
procedure(msg, severity);
    /* Prints and accounts for all error messages */
    declare msg character, severity fixed;

    error_count = error_count + 1;
    /* If listing is suppressed, force printing of this line */
    if ~control(byte('K')) then
        output = i_format(card_count, 6) || ' |' || buffer || '|';
    output = pad('', text_used + 8) || stroke;
    output = '*** ERROR, ' || msg ||
        '.  Last previous error was detected on line ' ||
        previous_error || '.  ***';
    previous_error = card_count;
    if severity > 0 then compiling = FALSE;
end error;

/*                   Card image handling procedure                      */

get_card:
procedure;
    /* Does all card reading and listing                                 */
    declare i fixed, rest character;

    buffer = input(source_file);
    if length(buffer) = 0 then do;
            /* In xcom, call error here */
            compiling = FALSE;
            s = '';
            do i = 0 to 1;
                s = s || ' eof';
            end;
            buffer = pad(s, 80);
        end;
    else card_count = card_count + 1;
    if margin_chop > 0 & margin_chop < length(buffer) then
        do;  /* The margin control from dollar | */
            rest = substr(buffer, margin_chop);
            buffer = substr(buffer, 0, margin_chop);
        end;
    else rest = '';
    text = buffer;
    text_limit = length(text) - 1;
    text_used = 0;
    if control(byte('K')) then
        output = i_format(card_count, 6) || ' |' || buffer || '|' || rest;
    cp = 0;
end get_card;

/*                The scanner procedures              */

char:
procedure;
    /* Used for strings to avoid card boundary problems */
    cp = cp + 1;
    if cp <= text_limit then return;
    call get_card;
end char;

scan:
procedure;
    declare i fixed;
    declare (s1, s2) fixed;

    bcd = '';
    do FOREVER;
        if cp > text_limit then call get_card;
        else
            do;  /* Discard last scanned value */
                text_used = text_used + cp;
                text_limit = text_limit - cp;
                text = substr(text, cp);
                cp = 0;
            end;
        /*  Branch on next character in text */
        do case chartype(byte(text));

            /*  Case 0  */

            /* Illegal characters fall here  */
            if macroend = FALSE then
                call error('Illegal character: ' || substr(text, 0, 1), 0);

            /*  Case 1  */

            /*  Blank  */
            do;
                cp = 1;
                do while byte(text, cp) = byte(' ') & cp <= text_limit;
                    cp = cp + 1;
                end;
                cp = cp - 1;
            end;

            /*  Case 2  */
            /*  A '|' may be orsymbol or catenate  */

            do;
                call char;
                if byte(text, cp) = byte(stroke) then do;
                        call char;
                        token = catenate;
                    end;
                else token = orsymbol;
                return;
            end;

            /*  Case 3  */
            /*  Bit string constant  */

            do;
                token = string;
                call char;  /* To skip past initial bitquote */
                do while byte(text, cp) ~= byte(bitquote);
                    call char;
                    if compiling = FALSE then return;
                end;
                call char;  /* To skip past the terminal bitquote */
                return;
            end;

            /*  Case 4  */

            do FOREVER;  /* A letter:  Identifiers and reserved words */
                do cp = cp + 1 to text_limit;
                    if not_letter_or_digit(byte(text, cp)) then
                        do;  /* End of identifier  */
                            if cp > 0 then bcd = bcd || substr(text, 0, cp);
                            s1 = length(bcd);
                            if s1 > 1 then
                                if s1 <= reserved_limit then
                                    /* Check for reserved words */
                                    do i = vt_index(s1) to vt_index(s1 + 1) - 1;
                                        if case_compare(bcd, v(i)) = 0 then
                                            do;
                                                token = i;
                                                return;
                                            end;
                                    end;
                            /* Reserved words exit higher: therefore <identifier> */
                            token = ident;
                            return;
                        end;
                end;
                /*  End of card  */
                bcd = bcd || text;
                call get_card;
                cp = -1;
            end;

            /*  Case 5  */

            do;      /*  Digit:  A number  */
                token = number;
                do FOREVER;
                    do cp = cp to text_limit;
                        s1 = byte(text, cp);
                        if chartype(s1) ~= 5 then return;
                    end;
                    call get_card;
                end;
            end;

            /*  Case 6  */

            do;      /*  A /:  May be divide or start of comment  */
                call char;
                if byte(text, cp) ~= byte('*') then
                    do;
                        if byte(text, cp) = byte('/') then do;
                                cp = text_limit + 1;
                            end;
                        token = divide;
                        return;
                    end;
                /* We have a comment  */
                s1, s2 = byte(' ');
                do while s1 ~= byte('*') | s2 ~= byte('/');
                    if s1 = byte('$') then
                        do;  /* A control character  */
                            control(s2) = ~control(s2);
                            if s2 = byte('|') then
                                if control(s2) then
                                    margin_chop = text_used + cp;
                                else
                                    margin_chop = 0;
                        end;
                    s1 = s2;
                    call char;
                    s2 = byte(text, cp);
                    if compiling = FALSE then do;
                            token = 0;
                            return;
                        end;
                end;
            end;

            /*  Case 7  */
            do;      /*  Special characters  */
                token = tx(byte(text));
                cp = 1;
                return;
            end;

            /*  Case 8 --  A string quote  */

            do FOREVER;
                call char;  /* Skip over the initial quote */
                if macrodef then do;
                        /* If this is a macro then scan the string */
                        macrodef = FALSE;
                        macroend = TRUE;
                        token = 0;
                        return;
                    end;
                token = string;
                if macroend then
                    if byte(text, cp) ~= byte(quote) then do;
                            macroend = FALSE;
                            call char;
                            return;
                        end;
                do while byte(text, cp) ~= byte(quote);
                    bcd = bcd || substr(text, cp, 1);
                    call char;
                    if compiling = FALSE then return;
                end;
                call char;  /* Skip over the terminal quote */
                if byte(text, cp) ~= byte(quote) then do;
                        return;
                    end;
            end;

            /*  Case 9 --  A TAB  */

            do;
                text_used = text_used + 8 - (text_used mod 8);
                text_limit = text_limit - 1;
                text = substr(text, 1);
                cp = -1;
            end;

        end;     /* Of case on chartype  */
        cp = cp + 1;  /* Advance scanner and resume search for token  */
    end;
end scan;

/*  H A S H   A D D R E S S E D   S Y M B O L   T A B L E   L O O K U P  */

nullout:
procedure(maxindex);
    declare (i, maxindex) fixed;

    do i = 0 to 255;
        if bucket(i) >= maxindex then
            bucket(i) = NULL;
    end;
    do i = 1 to maxindex - 1;
        if sy_link(i) >= maxindex then
            sy_link(i) = NULL;
    end;
end nullout;

id_lookup:
procedure;
    declare (i, l, sy_ptr) fixed;

    not_found = FALSE;
    l = lex_level;
    i = length(bcd) - 1;
    i = byte(bcd, i) + byte(bcd) + i;
    do while l >= 0;
        sy_ptr = bucket((i + proc_stack(l)) & "FF");
        do while sy_ptr ~= NULL;
            if syt(sy_ptr) = bcd then do;
                    /* Found identifier */
                    ref_link(last_ref(sy_ptr)) = nextref;
                    line_#(nextref) = card_count;
                    last_ref(sy_ptr) = nextref;
                    nextref = nextref + 1;
                    return;
                end;
            sy_ptr = sy_link(sy_ptr);
        end;
        l = l - 1;
    end;
    /* Search the pre-defined symbols and ignore case */
    do sy_ptr = 0 to n_predeclared_symb;
        if case_compare(syt(sy_ptr), bcd) = 0 then do;
                /* Found identifier */
                ref_link(last_ref(sy_ptr)) = nextref;
                line_#(nextref) = card_count;
                last_ref(sy_ptr) = nextref;
                nextref = nextref + 1;
                return;
            end;
    end;
    not_found = TRUE;
end id_lookup;

enter:
procedure(symb, line) fixed;
    declare (i, line) fixed;
    declare symb character, sy_ptr fixed;

    i = length(symb) - 1;
    i = (byte(symb, i) + byte(symb) + i + proc_stack(lex_level)) & "FF";
    sy_ptr = bucket(i);
    if sy_ptr ~= NULL then do;
        search_somemore:  /* Do until sy_link(sy_ptr) = NULL */
            if sy_link(sy_ptr) ~= NULL then do;
                    sy_ptr = sy_link(sy_ptr);
                    goto search_somemore;
                end;
            ndecsy, sy_ptr, sy_link(sy_ptr) = ndecsy + 1;
        end;
    else
        ndecsy, sy_ptr, bucket(i) = ndecsy + 1;
    syt(sy_ptr) = symb;
    last_ref(sy_ptr), ref_ptr(sy_ptr) = nextref;
    line_#(nextref) = line;
    nextref = nextref + 1;
    owner(sy_ptr) = proc_stack(lex_level);
    return sy_ptr;
end enter;

sy_dump:
procedure;
    declare (p, sy_listhead) fixed,
        (ref_message, decl_message) character,
        call_print_header bit(1);

dump_list:
    procedure(list_ptr);
        declare list_ptr bit(16);
        declare (line, numeral) character;

        line = substr(x70, 0, 40);
        do while list_ptr ~= NULL;
            numeral = line_#(list_ptr);
            if length(line) + length(numeral) > 120 then do;
                    output = line;
                    line = substr(x70, 0, 40);
                end;
            line = line || numeral || x1;
            list_ptr = ref_link(list_ptr);
        end;
        if length(line) > 40 then output = line;
    end dump_list;

sort_symbols:
    procedure;
        declare i bit(16);

    insert:
        procedure;
            declare ptr bit(16);

            if syt(i) < syt(sy_listhead) then do;
                    sy_link(i) = sy_listhead;
                    sy_listhead = i;
                    return;
                end;
            ptr = sy_listhead;
            do while sy_link(ptr) ~= NULL;
                if syt(i) < syt(sy_link(ptr)) then do;
                        sy_link(i) = sy_link(ptr);
                        sy_link(ptr) = i;
                        return;
                    end;
                ptr = sy_link(ptr);
            end;
            sy_link(ptr) = i;
        end insert;

        sy_listhead = NULL;
        do i = procmark to ndecsy;
            if length(syt(i)) > 0 then do;
                    syt(i) = pad(syt(i), 32);
                    sy_link(i) = NULL;
                    if sy_listhead = NULL then
                        sy_listhead = i;
                    else call insert;
                end;
        end;
    end sort_symbols;

print_header:
    procedure;
        EJECT_PAGE;
        if lex_level > 1 then
            output = substr(x70, 0, 32) ||
                'X P L   Cross-reference table for local identifiers of procedure '
                || syt(procmark - 1);
        else do;
                output = substr(x70, 0, 32) ||
                    'X P L   Cross-reference table of global identifiers';
                output = end_message;
            end;
        DOUBLE_SPACE;
        output = '     Symbol table contents';
        DOUBLE_SPACE;
        call_print_header = FALSE;
    end print_header;

    call sort_symbols;
    call_print_header = TRUE;
    do while sy_listhead ~= NULL;
        p = ref_link(ref_ptr(sy_listhead));
        if control(byte('Z')) = 0 | p = NULL then
            do;
                if call_print_header then call print_header;
                if line_#(ref_ptr(sy_listhead)) > 0 then
                    decl_message = ' declared on line ' || line_#(ref_ptr(sy_listhead));
                else decl_message = ' predeclared';
                if p = NULL then ref_message = ' and never referenced';
                else ref_message = ' and referenced on lines';
                output = syt(sy_listhead) || decl_message || ref_message;
                call dump_list(p);
            end;
        sy_listhead = sy_link(sy_listhead);
    end;
    if call_print_header = FALSE then DOUBLE_SPACE;
end sy_dump;

/*                       Initialization                                     */

initialization:
procedure;
    declare (i, j) fixed, t character;

    do i = 0 to NT;         /* Only need 1 to NT, except in this example*/
        s = v(i);
        if s = '<number>' then number = i;  else
        if s = '<identifier>' then do;
                ident = i;
                tokentype(i) = 1;
            end;
        else
        if s = '/' then divide = i;  else
        if s = '<string>' then string = i;  else
        if s = '|' then orsymbol = i;  else
        if s = '||' then catenate = i;  else
        if s = 'procedure' then tokentype(i) = 2;  else
        if s = 'do' then tokentype(i) = 3;  else
        if s = 'end' then tokentype(i) = 4;  else
        if s = 'literally' then tokentype(i) = 5;  else
        if s = 'eof' then tokentype(i) = 6;  else
            ;
    end;
    if ident = NT then reserved_limit = length(v(NT - 1));
    else reserved_limit = length(v(NT));
    chartype(byte(' ')) = 1;
    chartype(byte(quote)) = 8;
    chartype(byte("(c)\t")) = 9;  /* TAB */
    chartype(byte(bitquote)) = 3;
    do i = 0 to 255;
        not_letter_or_digit(i) = TRUE;
        lower(i) = i;
    end;
    do i = 0 to length(upper_case) - 1;
        lower(byte(upper_case, i)) = byte(lower_case, i);
    end;
    text = upper_case || lower_case || special_case;
    do i = 0 to length(text) - 1;
        j = byte(text, i);
        tx(j) = i;
        not_letter_or_digit(j) = FALSE;
        chartype(j) = 4;
    end;
    do i = 0 to 9;
        j = byte('0123456789', i);
        not_letter_or_digit(j) = FALSE;
        chartype(j) = 5;
    end;
    do i = vt_index(1) to vt_index(2) - 1;
        j = byte(v(i));
        tx(j) = i;
        chartype(j) = 7;
    end;
    chartype(byte('/')) = 6;
    chartype(byte(stroke)) = 2;

    do i = 0 to 255;
        bucket(i) = NULL;
    end;
    do i = 0 to SYTSIZE;
        sy_link(i), ref_ptr(i), last_ref(i) = NULL;
    end;
    do i = 0 to LIST_SIZE;
        ref_link(i) = NULL;
    end;
    lex_level, procedure_number = 1;
    ndecsy = 0;
    do while length(syt(ndecsy)) > 0;
        ref_ptr(ndecsy), last_ref(ndecsy) = nextref;
        nextref = nextref + 1;
        t = syt(ndecsy);
        i = length(t) - 1;
        i = (byte(t, i) + byte(t) + i) & "FF";
        if bucket(i) = NULL then bucket(i) = ndecsy;
        else do;
                i = bucket(i);
                do while sy_link(i) ~= NULL;
                    i = sy_link(i);
                end;
                sy_link(i) = ndecsy;
            end;
        ndecsy = ndecsy + 1;
    end;
    procmark = ndecsy;
    n_predeclared_symb, ndecsy = ndecsy - 1;

    /* First set up global variables controlling scan, then call it */
    cp = 0;  text_limit = -1;
    text = '';
    compiling = TRUE;  /* You can abort compilation by setting FALSE */
    call scan;
end initialization;

build_cross_ref_table:
procedure;
    declare i fixed;

    do while compiling;
        do case tokentype(token);

            /*  Case 0 -- not an identifier  */
            ;

            /*  Case 1 -- identifier  */
            do;
                call id_lookup;
                if not_found then do;
                        call enter(bcd, card_count);
                        if ndecsy >= SYTSIZE then
                            call error('Symbol table overflow', 1);
                    end;
                if nextref >= LIST_SIZE then
                    call error('Reference table overflow', 1);
            end;

            /*  Case 2 -- 'procedure'  */
            do;
                stackmark(lex_level) = procmark;
                lex_level = lex_level + 1;
                procedure_number, proc_stack(lex_level) = procedure_number + 1;
                endcount(lex_level) = 0;
                procmark = ndecsy + 1;
                if lex_level >= MAXNEST then
                    call error('Procedure nest table overflow', 1);
            end;

            /*  Case 3 -- 'do'  */
            endcount(lex_level) = endcount(lex_level) + 1;

            /*  Case 4 -- 'end'  */
            if endcount(lex_level) = 0 then do;
                    if control(byte('P')) then call sy_dump;
                    if lex_level > 0 then
                        lex_level = lex_level - 1;
                    call nullout(procmark);
                    do i = procmark to ndecsy;
                        owner(i) = 0;
                        syt(i) = x1;  sy_link(i) = NULL;
                        ref_ptr(i), last_ref(i) = NULL;
                    end;
                    ndecsy = procmark - 1;
                    procmark = stackmark(lex_level);
                end;
            else
                endcount(lex_level) = endcount(lex_level) - 1;

            /*  Case 5 -- 'literally'  */
            macrodef = TRUE;

            /*  Case 6 -- EOF */
            compiling = FALSE;

        end;
        call scan;
    end;
end build_cross_ref_table;

/*
**      Print a brief description of how to use the program
*/
usage:
procedure;
    output(1) = 'Usage: ' || argv(0) || ' [-options] <source-file>';
    output(1) = '   <source-file>    Input file';
    output(1) = 'Options:';
    output(1) = '   K          List the program';
    output(1) = '   P          Cross-reference local variables of procedures';
    output(1) = '   Z          Only display variables that were never used';
    output(1) = 'Comment toggles';
    output(1) = '   |          Margin chop';
end usage;

/*
**	Process command line options
*/
call_usage:
procedure bit(1);
    declare (i, j, ch) fixed;
    declare uc(255) bit(1);

    do i = 0 to 255;
        uc(i) = FALSE;
    end;
    do i = 0 to length(upper_case) - 1;
        uc(byte(upper_case, i)) = TRUE;
    end;
    do i = 1 to argc - 1;
        if byte(argv(i), 0) = byte('-') then
            do j = 1 to length(argv(i)) - 1;
                ch = byte(argv(i), j);
                if uc(ch) then control(ch) = TRUE;
                else return 1;
            end;
        else
            do;
                source_file = xfopen(argv(i), 'r');
                if source_file < 0 then do;
                        output(1) = 'Open file error: ' || argv(i);
                        return 1;
                    end;
            end;
    end;
    return 0;
end call_usage;

/* Execution starts here */

if call_usage then
    do;
        call usage;
        return 1;
    end;

call initialization;  /* Which makes an initial call to scan */
call build_cross_ref_table;
if lex_level > 1 then do;
        end_message = end_message || 'Table may include some extraneous ' ||
            'local identifiers because of one or more missing ''end'' symbols';
        lex_level = 1;
    end;
procmark = 0;
call sy_dump;
return 0;

eof eof eof
