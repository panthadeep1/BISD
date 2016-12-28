#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <map>
#include <cmath>
#include <cstring>
#include <cassert>
#include <string>
#include <queue>
#include <algorithm>
#include <ctime>
#include <fstream>
#include <unistd.h>
#include <ios>
#include <sstream>
#include <queue>
using namespace std;

void input_file_orig(string);
void norm_log(string);
void norm_square_root(string);
void norm_div(string);
void norm_space(string);
void norm_binary(string);

int main(int argc,char*argv[])
{

    string fname;
    int normr;
    fname=argv[1];
  

    ifstream in(fname.c_str());
    if(!in){cout<<"Cannot open the file"<<fname;exit(0);}
    if(in){cout<<"The input file is:"<<fname<<endl;}

    cout<<"Options:\n";
    cout<<"0-no normalization\n";
    cout<<"1-normalization by logarithm scale\n";
    cout<<"2-normalization by square-root method\n";
    cout<<"3-diving by the n-th power of 10\n";
    cout<<"4-CSV to space separated\n";
    cout<<"5-Convert the input data file nrow X ncol to binary file\n";
    cout<<"6-Convert NN_matrix to binary file\n";
    cout<<"7-Convert Similarity matrix to binary file\n";
    cout<<"8-Convert core/non-core to binary file\n";
    cout<<"9-Convert Clusters to binary file\n";

    cout<<"Enter the option:";
    cin>>normr;

    switch(normr)
    {
      case 0:
        input_file_orig(fname);
        break;
      case 1:
        norm_log(fname);
        break;
      case 2:
        norm_square_root(fname);
        break;
      case 3:
        norm_div(fname);
        break;
      case 4:
        norm_space(fname);
        break;
      case 5:
        norm_binary(fname);
        break;  
      default:
        input_file_orig(fname);
        break;
    }

    return 0;


}// end main

//Keep Original input file........
void input_file_orig(string fname)
{
   string input_file = fname; 
   string input_file_proc = fname + "_proc";
   ifstream in_normr(input_file.c_str());
		   ofstream out_normr(input_file_proc.c_str());
		   if(!in_normr){cout<<"Cannot open'"<<input_file<<"'\n";exit(0);}

		   string str,word;
		   long double sum=0,norm_val=0;
		   unsigned long int z=0; //unsigned
		   vector<string>vec;
		   vector<string>:: iterator v;

                   getline(in_normr,str); 
                   out_normr<<str;
		   while(getline(in_normr,str))
		   {
				     stringstream ss(str);
				     
				     while (ss >> word)
					{
					    
				            vec.push_back(word);

					    if (ss.peek() == ' ')
						ss.ignore();
					}


				     v=vec.begin();
				     for(int i=0;i<vec.size();i++)
				      {
				        
				         out_normr<<vec[i];
				         if((v+1)!=vec.end())
				         out_normr<<" ";
				         v++;
				      } 
				    

				out_normr<<endl;

				  while (!vec.empty())
				  {
				     vec.pop_back();
				  }
				  sum=0;z=0;

		   } //end while

		  in_normr.close();
		  out_normr.close();


}// end func


// Normalize by taking square root.......
void norm_square_root(string fname)
{
   string input_file = fname; 
   string input_file_proc = fname + "_proc";
   ifstream in_normr(input_file.c_str());
		   ofstream out_normr(input_file_proc.c_str());
		   if(!in_normr){cout<<"Cannot open'"<<input_file<<"'\n";exit(0);}

		   string str,word;
		   long double sum=0,norm_val=0;
		   long double z=0; //unsigned
		   vector<string>vec;
		   vector<string>:: iterator v;

                   getline(in_normr,str);
                   out_normr<<str<<endl;
		   while(getline(in_normr,str))
		   {
				     stringstream ss(str);
				     
				     while (ss >> word)
					{
					    
				            vec.push_back(word);

					    if (ss.peek() == ' ')
						ss.ignore();
					}

                                     char *ptr;
                                     for(int i=0;i<vec.size();i++)
				      {
				         z=strtold(vec[i].c_str(),&ptr);
				         sum = sum + (z*z);
				        
				      } 

				      sum=sqrt(sum);

                                     z=0; 
				     v=vec.begin();
				     for(int i=0;i<vec.size();i++)
				      {
				         z=strtold(vec[i].c_str(),&ptr);
				         norm_val = z/sum;
				         out_normr<<norm_val;
				         if((v+1)!=vec.end())
				         out_normr<<" ";
				         v++;
				      } 
				    

				out_normr<<endl;

				  while (!vec.empty())
				  {
				     vec.pop_back();
				  }
				  sum=0;z=0;

		   } //end while

		  in_normr.close();
		  out_normr.close();



}// end func


// Normalize by diving by powers of 10
void norm_div(string fname)
{
   string input_file = fname; 
   string input_file_proc = fname + "_proc";
   ifstream in_normr(input_file.c_str());
		   ofstream out_normr(input_file_proc.c_str());
		   if(!in_normr){cout<<"Cannot open'"<<input_file<<"'\n";exit(0);}

		   string str,word;
		   long double sum=0,norm_val=0;
		   long double z=0; //unsigned
		   vector<string>vec;
		   vector<string>:: iterator v;

                   getline(in_normr,str);
                   out_normr<<str<<endl;
		   while(getline(in_normr,str))
		   {
				     stringstream ss(str);
				     
				     while (ss >> word)
					{
					    
				            vec.push_back(word);

					    if (ss.peek() == ' ')
						ss.ignore();
					}

                                     char *ptr;                                     
                                     z=0; 
				     v=vec.begin();
				     for(int i=0;i<vec.size();i++)
				      {
				         z=strtold(vec[i].c_str(),&ptr);
				         norm_val = z/pow(10.0,5.0);
				         out_normr<<norm_val;
				         if((v+1)!=vec.end())
				         out_normr<<" ";
				         v++;
				      } 
				    

				out_normr<<endl;

				  while (!vec.empty())
				  {
				     vec.pop_back();
				  }
				  sum=0;z=0;

		   } //end while

		  in_normr.close();
		  out_normr.close();



}

void norm_log(string fname)
{
   string input_file = fname; 
   string input_file_proc = fname + "_proc";
   ifstream in_normr(input_file.c_str());
		   ofstream out_normr(input_file_proc.c_str());
		   if(!in_normr){cout<<"Cannot open'"<<input_file<<"'\n";exit(0);}

		   string str,word;
		   long double sum=0,norm_val=0;
		   long double z=0; //unsigned
		   vector<string>vec;
		   vector<string>:: iterator v;

                   getline(in_normr,str);
                   out_normr<<str<<endl;
		   while(getline(in_normr,str))
		   {
				     stringstream ss(str);
				     
				     while (ss >> word)
					{
					    
				            vec.push_back(word);

					    if (ss.peek() == ' ')
						ss.ignore();
					}

                                     char *ptr;                                     
                                     z=0; 
				     v=vec.begin();
				     for(int i=0;i<vec.size();i++)
				      {
				         z=strtold(vec[i].c_str(),&ptr);
				         norm_val = log10(z);
				         out_normr<<norm_val;
				         if((v+1)!=vec.end())
				         out_normr<<" ";
				         v++;
				      } 
				    

				out_normr<<endl;

				  while (!vec.empty())
				  {
				     vec.pop_back();
				  }
				  sum=0;z=0;

		   } //end while

		  in_normr.close();
		  out_normr.close();



}


//To convert CSV to space separated values...
void norm_space(string fname)
{
   string input_file = fname; 
   string input_file_proc = fname + "_proc";
   ifstream in_normr(input_file.c_str());
		   ofstream out_normr(input_file_proc.c_str());
		   if(!in_normr){cout<<"Cannot open'"<<input_file<<"'\n";exit(0);}

		   string str,word;
                   string oldstring(","),newstring(" ");
 
		   long double sum=0,norm_val=0;
		   long double z=0; //unsigned
		   vector<string>vec;
		   vector<string>:: iterator v;

                   size_t pos = 0;
                   getline(in_normr,str);
                   out_normr<<str<<endl;
		   while(getline(in_normr,str))
		   {


		         while ((pos = str.find(oldstring, pos)) != std::string::npos) 
                             {
				 str.replace(pos, oldstring.length(), newstring);
				 pos += newstring.length();
					      
		             }

		              pos=0;
		             	              
		              out_normr<<str;
		              if(!in_normr.eof())
		              out_normr<<endl;

		   } //end while

		  in_normr.close();
		  out_normr.close();



}// end func

void norm_binary(string fname)
{
  string input_file = fname;
  string output_file_bin = fname + ".bin";
  ifstream in(input_file.c_str());
  if(!in){cout<<"Cannot open'"<<input_file<<"'\n";exit(0);}
  ofstream out(output_file_bin.c_str(),ios::out|ios::binary);
  if(!out){cout<<"Cannot open'"<<output_file_bin<<"'\n";exit(0);}

    int nrow,ncol,nrow1,ncol1;
    long double temp,temp_read;
    string str,word;
    char *ptr;

    getline(in,str);
    stringstream ss(str);
    ss>>nrow>>ncol; 
    //cout<<nrow<<ncol;

    //Write the nrow.ncol values to binary file
    out.write((char*)&nrow,sizeof(nrow));
    out.write((char*)&ncol,sizeof(ncol));

    //Write the rest of the values
    while(getline(in,str))
    {
       stringstream ss(str);
       while(ss >> word)
       {
          temp = strtold(word.c_str(),&ptr);
          out.write((char*)&temp,sizeof(temp));
       }

    }
    out.close();
    in.close();

}































