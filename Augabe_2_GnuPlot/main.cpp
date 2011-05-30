#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <unistd.h> 



using namespace std;

int numplots = 3;
char*** plots;
int pid;
int pc[2];

void generatePlots()
{
	plots = new char**[numplots];
	
	plots[0] = new char*[13];
	plots[0][0] = "set dummy u,v";
	plots[0][1] = "set key bmargin center horizontal Right noreverse enhanced autotitles nobox";
	plots[0][2] = "set parametric";
	plots[0][3] = "set view 50, 30, 1, 1";
	plots[0][4] = "set isosamples 50, 20";
	plots[0][5] = "set hidden3d offset 1 trianglepattern 3 undefined 1 altdiagonal bentover";
	plots[0][6] = "set ticslevel 0";
	plots[0][7] = "set title \"Interlocking Tori\" ";
	plots[0][8] = "set urange [ -3.14159 : 3.14159 ] noreverse nowriteback";
	plots[0][9] = "set vrange [ -3.14159 : 3.14159 ] noreverse nowriteback";
	plots[0][10] = "set zrange [ * : * ] noreverse nowriteback  # (currently [-3.00000:1.50000] )";
	plots[0][11] = "splot cos(u)+.5*cos(u)*cos(v),sin(u)+.5*sin(u)*cos(v),.5*sin(v) with lines,       1+cos(u)+.5*cos(u)*cos(v),.5*sin(v),sin(u)+.5*sin(u)*cos(v) with lines";
	plots[0][12] = "reset";

	plots[1] = new char*[16];
	plots[1][0] = "set view map";
	plots[1][1] = "set samples 101, 101";
	plots[1][2] = "set isosamples 2, 2";
	plots[1][3] = "set style data pm3d";
	plots[1][4] = "set style function pm3d";
	plots[1][5] = "set xtics border in scale 1,0.5 mirror norotate  offset character 0, 0, 0 2";
	plots[1][6] = "set noytics";
	plots[1][7] = "set noztics";
	plots[1][8] = "set title \"set palette model HSV rrgbformulae 3,2,2\" ";
	plots[1][9] = "set xrange [ -10.0000 : 10.0000 ] noreverse nowriteback";
	plots[1][10] = "set cbrange [ -10.0000 : 10.0000 ] noreverse nowriteback";
	plots[1][11] = "set pm3d implicit at b";
	plots[1][12] = "set palette positive nops_allcF maxcolors 0 gamma 1.5 color model HSV ";
	plots[1][13] = "set palette rgbformulae 3, 2, 2";
	plots[1][14] = "splot x";
	plots[1][15] = "reset";

	plots[2] = new char*[25];
	plots[2][0] = "set clip two";
	plots[2][1] = "set style fill transparent solid 0.50 noborder";
	plots[2][2] = "set key title \"Gaussian Distribution\"";
	plots[2][3] = "set key inside left top vertical Left reverse enhanced autotitles nobox";
	plots[2][4] = "set key noinvert samplen 1 spacing 1 width 0 height 0 ";
	plots[2][5] = "set style function filledcurves y1=0";
	plots[2][6] = "set title \"Transparent filled curves\" ";
	plots[2][7] = "set rrange [ * : * ] noreverse nowriteback  # (currently [8.98847e+307:-8.98847e+307] )";
	plots[2][8] = "set trange [ * : * ] noreverse nowriteback  # (currently [-5.00000:5.00000] )";
	plots[2][9] = "set xrange [ -5.00000 : 5.00000 ] noreverse nowriteback";
	plots[2][10] = "set x2range [ * : * ] noreverse nowriteback  # (currently [-5.00000:5.00000] )";
	plots[2][11] = "set yrange [ 0.00000 : 1.00000 ] noreverse nowriteback";
	plots[2][12] = "set y2range [ * : * ] noreverse nowriteback  # (currently [0.00000:1.00000] )";
	plots[2][13] = "set cbrange [ * : * ] noreverse nowriteback  # (currently [8.98847e+307:-8.98847e+307] )";
	plots[2][14] = "unset colorbox";
	plots[2][15] = "Gauss(x,mu,sigma) = 1./(sigma*sqrt(2*pi)) * exp( -(x-mu)**2 / (2*sigma**2) )";
	plots[2][16] = "d1(x) = Gauss(x, 0.5, 0.5)";
	plots[2][17] = "d2(x) = Gauss(x,  2.,  1.)";
	plots[2][18] = "d3(x) = Gauss(x, -1.,  2.)";
	plots[2][19] = "GPFUN_Gauss = \"Gauss(x,mu,sigma) = 1./(sigma*sqrt(2*pi)) * exp( -(x-mu)**2 / (2*sigma**2) )\"";
	plots[2][20] = "GPFUN_d1 = \"d1(x) = Gauss(x, 0.5, 0.5)\"";
	plots[2][21] = "GPFUN_d2 = \"d2(x) = Gauss(x,  2.,  1.)\"";
	plots[2][22] = "GPFUN_d3 = \"d3(x) = Gauss(x, -1.,  2.)\"";
	plots[2][23] = "plot d1(x) fs solid 1.0 lc rgb \"forest-green\" title \"μ =  0.5 σ = 0.5\",      d2(x) lc rgb \"gold\" title \"μ =  2.0 σ = 1.0\",      d3(x) lc rgb \"red\" title \"μ = -1.0 σ = 2.0\"";
	plots[2][24] = "reset";
}

void startPlot(int i)
{	
	if(i >= 0 && numplots >= i)
	{
		cout << "Starting Plot No. " << i << endl;
		
		int n=0;
		char st[5];
		
		while(plots[i-1][n] && strncmp(plots[i-1][n], "reset", 5))
		{
			write(pc[1], plots[i-1][n], strlen(plots[i-1][n]));
			write(pc[1], "\n", 1);
			n++;
		}
		write(pc[1], plots[i-1][n], strlen(plots[i-1][n]));
		write(pc[1], "\n", 1);
		
	}
	else
		cout << "Plot No. " << i << " is out of range" << endl;
}

void parent()
{
	close(pc[0]);
	generatePlots();
	char c = 0;
	
	do
	{
		if(c > '0' && c < '5')
			startPlot(atoi(&c));
		cout << endl << "Press 1, 2 or 3 and <ENTER> to start a gnuplot-graph or q to exit:" << endl;
		
		c = cin.get();
		cin.ignore(1000, '\n');
    }
	while(c != 'q');
	
	system("killall gnuplot");
	cout << "Parent is dead!" << endl;
}

void child()
{
	cout << "Child starting!" << endl;
	close(2);
	close(1);
	close(0);
	close(pc[1]);
	dup2(pc[0], 0);
	close(pc[0]);

	execvp("gnuplot", NULL);
	
	cout << "Child is dead!" << endl;
	exit(1);
}

int main(int argc, char *argv[])
{
	FILE *fp;
	fp = popen("whereis gnuplot", "r");  		// gnuplot:
	char line[100];
	fgets(line, sizeof line, fp);
	pclose(fp);
	if(!strlen(line) > 9)
		return EXIT_FAILURE;
	
	if(pipe(pc) < 0)
	{
		cout << "Error using pipe()" << endl;
		return EXIT_FAILURE;
	}

	
	switch(pid = fork())
	{
		case -1:
			cout << "Error using fork()" << endl;
			return EXIT_FAILURE;
		case 0:
			child();
			break;
		default:
			parent();
	}
    return EXIT_SUCCESS;
}
