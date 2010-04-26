
/**
 **  detox64 v0.9
 **
 **  Detokenizes C64 BASIC programs and prints them to stdout.
 **  Written by Stian Soreng, http://jmp.no/
 **
 **  This was just a quick hack to batch convert some old
 **  C64 BASIC progams, without having to go by a C64 emulator.
 **  Support for other formats can be implemented on request.
 **
 **  This source was released under the GNU General Public License.
 **  For more details, see http://www.gnu.org/licenses/gpl.html
 **
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *tokens[] = {
	"END", "FOR", "NEXT", "DATA", "INPUT#", "INPUT", "DIM", "READ",
	"LET", "GOTO", "RUN", "IF", "RESTORE", "GOSUB", "RETURN", "REM",
	"STOP", "ON", "WAIT", "LOAD", "SAVE", "VERIFY",	"DEF", "POKE",
	"PRINT#", "PRINT", "CONT", "LIST", "CLR", "CMD", "SYS", "OPEN",
	"CLOSE", "GET", "NEW", "TAB(", "TO", "FN", "SPC(", "THEN",
	"NOT", "STEP", "+", "-", "*", "/", "^", "AND",
	"OR", ">", "=", "<", "SGN", "INT", "ABS", "USR", 
	"FRE", "POS", "SQR", "RND", "LOG", "EXP", "COS", "SIN",
	"TAN", "ATN", "PEEK", "LEN", "STR$", "VAL", "ASC", "CHR$",
	"LEFT$", "RIGHT$", "MID$"
};

unsigned char getByte( FILE *fp ) {
	unsigned char in = 0x00;
	if( fp ) 
		(void) fread( &in, 1, 1, fp );
	return( in );
}

enum {
	INPUT_BAS=1,
	INPUT_P00
};

int help() {
	printf( "Usage: detox64 [-f type] <filename>\n" );
	printf( "-f type: input file format name, possible values are:\n" );
	printf( "     -f bas    C64 BASIC format\n" );
	printf( "     -f p00    Emulator file\n" );
	return( 1 );
}

int main( int argc, char **argv ) {
	unsigned int lineno = 0;
	FILE *fp;
	int argptr=1;
	int input_type=0;
	int quoted=0;

	if( argc == 1 ) return( help() );

	if( strcmp(argv[argptr],"-f") == 0 ) {
		argptr++;

		if( strcmp( argv[argptr], "bas" ) == 0 ) {
			input_type = INPUT_BAS;
		} else if( strcmp( argv[argptr], "p00" ) == 0 ) {
			input_type = INPUT_P00;
		} else {
			return( help() );
		}

		argptr++;
	}

	fp = fopen( argv[argptr], "rb" );
	if( fp == NULL ) {
		printf( "Unable to open input file '%s' for reading\n",
			argv[argptr] );
		return( 2 );
	}

	if( input_type == INPUT_P00 ) {
		char tmp[7];
		(void) fread( tmp, 7, 1, fp );	
		if( strcmp( tmp, "C64File" ) == 0 ) {
			int x=0;
			for( x=0; x<19; x++ )
				(void) getByte( fp );
		} else {
			printf( "File format not recognized\n" );
			fclose( fp );
			return( 1 );
		}
	}

	if( ((getByte(fp))|(getByte(fp)<<8)) != 0x0801 ) {
		printf( "This is probably not a BASIC file.\n" );
		fclose( fp );
		return( 1 );
	}

	(void)getByte(fp);
	(void)getByte(fp);

	while( !feof(fp) ) {
		unsigned char ch = 0;
		lineno = getByte(fp)|(getByte(fp)<<8);
		printf( "%d ", lineno );
		ch = getByte( fp );
		while( (!feof(fp)) && (ch != 0x00) ) {
			if( ch == '"' ) quoted = !quoted;
			if( ch > 0x7f ) {
				if( quoted )
					printf( "[%03d]", ch );
				else
					printf( "%s", tokens[ch&0x7f] );
			} else {
				printf( "%c", ch );
			}
			ch = getByte( fp );
		}
		printf( "\n" );
		if( (getByte(fp)|(getByte(fp)<<8)) == 0x00 ) break;
	}
	fclose( fp );
	return( 0 );
}

