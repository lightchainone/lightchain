
#include <cfgflag.h>
#include <iostream>
#include <stdio.h>

int main(int argc, char **argv)
{
	comcfg::CmdOption flag;
	flag.init(argc, argv, "o:");

	if (flag.argSize() <= 1) {
		std::cout<<"usage: "<<argv[0]<<" file -o file.h"<<std::endl;
		return -1;
	}

	bsl::string name = flag.getArg(1);
	bsl::string out = flag.getArg(1);
	out.append(".h");
	if (flag.hasOption("o")) {
		out = flag["o"].to_bsl_string();
	} 
	std::cout<<"in: "<<name.c_str()<<" \t out: "
		<<out.c_str()<<std::endl;

	FILE *fin = fopen(name.c_str(), "r");
	if (fin == NULL) {
		std::cout<<"open "<<name.c_str()<<" to read error"<<std::endl;
		return -1;
	}
	FILE *fout = fopen(out.c_str(), "w");
	if (fout == NULL) {
		std::cout<<"open "<<out.c_str()<<" to write error"<<std::endl;
		return -1;
	}
	bsl::string header = "__";
	header.append(out);
	for (int i=0; i<(int)header.size(); ++i) {
		header[i] = toupper(header[i]);
		if (!isalpha(header[i]) && !isdigit(header[i])) {
			header[i] = '_';
		}
	}
	bsl::string sname = name;
	for (int i=0; i<(int)sname.size(); ++i) {
		sname[i] = tolower(sname[i]);
		if (!isalpha(sname[i]) && !isdigit(sname[i])) {
			sname[i] = '_';
		}
	}
	fprintf(fout, "#ifndef %s\n", header.c_str());
	fprintf(fout, "#define %s\n", header.c_str());
	fprintf(fout, "static const char *g_str_%s = \"", sname.c_str());
	int c = 0;
	while ((c=fgetc(fin)) != EOF) {
		if (c == '"') {
			fprintf(fout, "\\\"");
		} else if (c == '\n') {
			fprintf(fout, "\\n\\\n");
		} else {
			fprintf(fout, "%c", c);
		}
	}
	fprintf(fout, "\";\n");
	fprintf(fout, "#endif\n");

	fclose(fin);
	fclose(fout);

	return 0;
}


