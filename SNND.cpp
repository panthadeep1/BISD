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
#include <sys/time.h>
using namespace std;

void write_values(string);
void write_binary_values(string);
int link_strength(int,int);
void link_cluster(int,int);

int nrow,ncol;
int KNN,SNN,DENSITY;
double Time;

vector< vector<long double> >data_matrix;
vector< vector< pair<long double,int> > >NN_matrix; 
vector< vector<int> >NN_list;
vector< vector< pair<int,int> > >similarity_matrix;
vector<int>core;
vector<int>non_core;

typedef struct Clust{

     public:
      int status;
      int rep;
      int core;
}Cluster;
Cluster *cluster;
       


int main(int argc, char* argv[])
{
  double t1_data=(double)clock()/CLOCKS_PER_SEC;
  string input_file = argv[1];
  string input_file_bin = input_file+".bin";
  string input_file_string = argv[1];
 
  ifstream in(input_file_bin.c_str(),ios::in|ios::binary);
  if(!in)
  {cout<<input_file_bin<<" dataset file is not found check again!!"<<endl; exit(0);}
  cout<<"Processed Input file:"<<input_file_bin<<endl;

  //Read the nrow,ncol values from input binary file

    in.read((char*)&nrow,sizeof(int));
    in.read((char*)&ncol,sizeof(int));

    cout<<"Rows:"<<nrow<<";Columns:"<<ncol<<endl;
     data_matrix.resize(nrow);    

  //Read the rest of the values from the input binary file
  long double temp;
  
  for(int i=0;i<=nrow-1;i++)
    {
       for(int j=0;j<=ncol-1;j++)
       {
	 in.read((char*)&temp,sizeof(long double));
	 data_matrix[i].push_back(temp);
       }
    }
  double t2_data=(double)clock()/CLOCKS_PER_SEC;

   cout<<"The data matrix is:\n";
   /*for(int i=0;i<=nrow-1;i++)
   {
      for(int j=0;j<=ncol-1;j++)
      {
         cout<<data_matrix[i][j]<<" ";
      }
    
     cout<<endl;
   }*/
 
   in.close();
   
                                   //START of Code segment for Fixing the Parameters of the DB-SNN Algorithm

  ifstream knn("KNN_global.bin",ios::in|ios::binary);
  knn.read((char*)&KNN,sizeof(KNN));

  ifstream snn("SNN_global.bin",ios::in|ios::binary);
  snn.read((char*)&SNN,sizeof(SNN));

  ifstream density("DENSITY_global.bin",ios::in|ios::binary);
  density.read((char*)&DENSITY,sizeof(DENSITY));

  knn.close();snn.close();density.close();   

  cout<<"Algorithm parameters set:(KNN size="<<KNN<<" ; SNN threshold="<<SNN<<" ; DENSITY threshold="<<DENSITY<<")\n";
  cout<<"Running for #points: "<<nrow<<endl;


                             //**********Distance and Nearest Neighbor calculation************//


 double t1_NN=(double)clock()/CLOCKS_PER_SEC; 
 NN_matrix.resize(nrow);
 for(int i=0;i<=nrow-1;i++)
 NN_matrix[i].resize(KNN);

 vector< pair<long double,int> > nn_dist(nrow-1);

 NN_list.resize(nrow);
 for(int i=0;i<=nrow-1;i++)
 NN_list[i].resize(KNN);

 int l=0;
 long double dist_ij,sum=0,sum1=0,sum2=0;
 pair<long double,int> d_ij;

  

	for(int i=0;i<=nrow-1;i++)
        {
	    cout<<i<<endl;
            l=0;
                 for(int j=0;j<=nrow-1;j++)
		  {
                       sum=0;sum1=0;sum2=0;

			     if(j==i)
			      continue;

			     if(j!=i)
			     {
                               
			        for(int k=0;k<=ncol-1;k++)
				 sum += pow((data_matrix[i][k]-data_matrix[j][k]),2); //Euclidean Distance
                                 dist_ij=sqrt(sum);   //sqrt
                               
			   
			     }

		      //dist_ij=sqrt(sum);//sqrt                          
           	       nn_dist[l].first = dist_ij;
                       nn_dist[l].second = j;

		       l++;
                   }

                   sort(nn_dist.begin(),nn_dist.end());
                    for(int k=0;k<=KNN-1;k++)
                      {                              
                         NN_matrix[i][k].first = nn_dist[k].first;
                         NN_matrix[i][k].second = nn_dist[k].second;
                      }


        }

        for(int i=0;i<=nrow-1;i++)
          {    
	    for(int j=0;j<=KNN-1;j++)
	    { 
	      NN_list[i][j]=NN_matrix[i][j].second;
	     		      
	    }  

            sort(NN_list[i].begin(),NN_list[i].end());

          }

       double t2_NN=(double)clock()/CLOCKS_PER_SEC;
       cout<<"NN list calculated in: "<<(t2_NN-t1_NN)<<endl;    
         cout<<"The NN matrix is:\n";
          for(int i=0;i<=nrow-1;i++)
          {
		cout<<i<<"] ";    
		    for(int j=0;j<=KNN-1;j++)
		    { 
		      
		      cout<<NN_matrix[i][j].first<<"/"<<NN_matrix[i][j].second<<" ";
		      
		    }  
        
                    

                   for(int j=0;j<=KNN-1;j++)		         
		    cout<<NN_list[i][j]<<" ";		      
		     
		    cout<<endl;

          }

        
                                         //************Link, similarity matrix creation*************//

      double t1_sim=(double)clock()/CLOCKS_PER_SEC;
      similarity_matrix.resize(nrow);
      int common;  
      for(int i=0;i<=nrow-1;i++)
       {
           
		  for(int j=0;j<=KNN-1;j++)
		  {
		     int k=NN_list[i][j];		     
		     if(binary_search(NN_list[k].begin(),NN_list[k].end(),i))
                     {
		        common = link_strength(i,k);                       
                        if(common >= SNN)
                        {
                           similarity_matrix[i].push_back(pair<int,int>(k,common));

                        } 
                       
                     }
		     else
		      {
                         continue;
                      }

		  }

         
       }

      double t2_sim=(double)clock()/CLOCKS_PER_SEC;
      
      cout<<"\nSimilarity matrix calculated in: "<<(t2_sim-t1_sim)<<endl;
       cout<<"The Similarity Matrix is:\n";
       for(int i=0;i<=nrow-1;i++)
       {
		  cout<<i<<"] ";
		  for(int j=0;j<similarity_matrix[i].size();j++)
		  {
		     cout<<similarity_matrix[i][j].first<<"/";
		     cout<<similarity_matrix[i][j].second<<" ";
		  }
	       
		 cout<<endl;
       } 



                                       //*********Core/Non-Core pt. detection****************//

       //Core Non-Core detection
       double t1_core=(double)clock()/CLOCKS_PER_SEC;
       for(int i=0;i<=nrow-1;i++)
       {
          if(similarity_matrix[i].size() > DENSITY)
            core.push_back(i);
          else
           non_core.push_back(i); 
       }
       double t2_core=(double)clock()/CLOCKS_PER_SEC;
       cout<<"\nCore points:\n";
       for(int i=0;i<core.size();i++)
       {
         cout<<core[i]<<" ";

       }

       cout<<"\nNon-Core points:\n";
       for(int i=0;i<non_core.size();i++)
       {
         cout<<non_core[i]<<" ";

       }

       cout<<(t2_core-t1_core);

                                                 //************CLUSTER FORMATION**************//


       cout<<endl;
       double t1_cluster=(double)clock()/CLOCKS_PER_SEC;
       cluster = new Cluster[nrow];
       for(int i=0;i<=nrow-1;i++)
       {
          if(similarity_matrix[i].size() > DENSITY)// core pt. check
          {
             cluster[i].status=1;
             cluster[i].rep=i;
             cluster[i].core=1;
          }

          else
          {
             cluster[i].status=0;
             cluster[i].rep=-1;
             cluster[i].core=0;

          }
        
       }
       
       cout<<"Initial Clusters:\n";
       for(int i=0;i<=nrow-1;i++)
       {
             cout<<i<<"] ";
             cout<<cluster[i].status<<"/";
             cout<<cluster[i].rep<<"/";
             cout<<cluster[i].core<<endl;

       }

      //******Case When No Clusters are formed******//
       if(core.size()==0)
        {
          cout<<"\nNo additional clusters possible\n";
          cout<<"\nNoise points:\n";
	       double t1_noise=(double)clock()/CLOCKS_PER_SEC;      
	       int noise=0;
	       for(int i=0;i<=nrow-1;i++)
	       {
		  if(cluster[i].core==0 && cluster[i].rep==-1)
		   {
		     cout<<i<<" ";
		     noise++;    
		   }
	       }
	       double t2_noise=(double)clock()/CLOCKS_PER_SEC;
        
       cout<<endl<<"\nAlgorithm details:";
       cout<<endl<<"Core points: "<<core.size();
       cout<<endl<<"Non-Core points: "<<non_core.size(); 
       cout<<endl<<"Noise points: "<<noise; 
       cout<<endl<<"Clustered non-core points: "<<non_core.size()-noise; 
       cout<<endl<<"Data created in: "<<(t2_data-t1_data);
       cout<<endl<<"NN lists calculated in: "<<(t2_NN-t1_NN);
       cout<<endl<<"Similarity matrix calculated in: "<<(t2_sim-t1_sim);
       cout<<endl<<"Core/Non-core pts. calculated in: "<<(t2_core-t1_core);
       cout<<endl<<"Clusters formed in: 0";
       cout<<endl<<"Noise pts. calculated in: "<<(t2_noise-t1_noise);   
  
       double t1_write=(double)clock()/CLOCKS_PER_SEC;
       write_binary_values(input_file_bin);
       double t2_write=(double)clock()/CLOCKS_PER_SEC;
       cout<<endl<<"Writing values...: "<<(t2_write-t1_write);

       Time = (t2_data-t1_data)+(t2_NN-t1_NN)+(t2_sim-t1_sim)+(t2_core-t1_core)+0+(t2_noise-t1_noise)+(t2_write-t1_write);

       cout<<endl<<"Total time elapsed: "<<(t2_data-t1_data)+(t2_NN-t1_NN)+(t2_sim-t1_sim)+(t2_core-t1_core)+0+(t2_noise-t1_noise)+(t2_write-t1_write)<<endl;

       write_values(input_file_string);

        //Time taken
       ofstream  time_ni("Time_Ni",ios::app);
       time_ni<<Time<<endl;
       time_ni.close();

        //Memory Footprint
		     ofstream mem_ni("Mem_Ni",ios::app);
		     void process_mem_usage(double&, double&);
		     double vm, rss;
		     process_mem_usage(vm, rss);
		     mem_ni << vm <<" "<< rss << endl;

  
          exit(0);
        }





       //Core pt. cluster formation       
       vector<int>temp_cluster;
       int min;        
       for(int i=0;i<=nrow-1;i++)
       {
		 if(similarity_matrix[i].size() > DENSITY)// core pt. check
		 {
		             for(int j=0;j<similarity_matrix[i].size();j++)
		             {
		                if(similarity_matrix[similarity_matrix[i][j].first].size() > DENSITY)// core pt. check
		                {
		                   temp_cluster.push_back(cluster[similarity_matrix[i][j].first].rep);
		                }                  
		             }

                             if(temp_cluster.size() == 0)//(single core pt. cluster)no link with other core pts.
                             {
                                 cluster[i].status = 1;
                                 cluster[i].rep = i;
                                 cluster[i].core = 1;
                                 continue;
                             }

		             min = *min_element(temp_cluster.begin(),temp_cluster.end()); // find the min. rep. of all pts.
		             
		             if(cluster[i].rep < min)
		             { 
		                cluster[i].rep = cluster[i].rep;
                                cluster[i].status = 1;
		                for(int j=0;j<similarity_matrix[i].size();j++)
		                {
		                   if(similarity_matrix[similarity_matrix[i][j].first].size() > DENSITY)// core pt. check
				        {
                                           link_cluster(cluster[similarity_matrix[i][j].first].rep,cluster[i].rep);
				           cluster[similarity_matrix[i][j].first].rep = cluster[i].rep;
                                           cluster[similarity_matrix[i][j].first].status = 1; 
				        }

		                }

		             }

		             if(cluster[i].rep >= min)
		             { 
                                link_cluster(cluster[i].rep,min);
		                cluster[i].rep = min;
		                for(int j=0;j<similarity_matrix[i].size();j++)
		                {
		                   if(similarity_matrix[similarity_matrix[i][j].first].size() > DENSITY)// core pt. check
				        {
                                           link_cluster(cluster[similarity_matrix[i][j].first].rep,min);
				           cluster[similarity_matrix[i][j].first].rep = min;
                                           cluster[similarity_matrix[i][j].first].status = 1;
				        }

		                }

		             }

                             

		 }//end if 

              temp_cluster.erase(temp_cluster.begin(),temp_cluster.end());
                      

       }//end for

       //Non-Core pt. cluster formation
       priority_queue< pair<int,int> >nearest_core;
       int nearest;
       for(int i=0;i<=nrow-1;i++)
       {
		  if(similarity_matrix[i].size() <= DENSITY)// non-core pt. check
		  {
                     if(similarity_matrix[i].size() == 0)
                       continue;
                     
		     for(int j=0;j<similarity_matrix[i].size();j++)
		     {
		        if(similarity_matrix[similarity_matrix[i][j].first].size() > DENSITY)// core pt. check
		        nearest_core.push(pair<int,int>(similarity_matrix[i][j].second,similarity_matrix[i][j].first));		        
		     } 
                    
                     if(nearest_core.size() > 0){
                     nearest = nearest_core.top().second;
                     cluster[i].rep = cluster[nearest].rep;
                     cluster[i].status = 1;
                     } 

		  }//end if

            while(!nearest_core.empty())
            nearest_core.pop();

       }//end for

       double t2_cluster=(double)clock()/CLOCKS_PER_SEC;

       string cluster_file=input_file_string+".cluster";
       ofstream out_cluster(cluster_file.c_str());
       if(!out_cluster)
          {cout<<"Cannot open the file cluster file:"<<cluster_file; exit(0);}
 
        int i;
        out_cluster<<"Points Status Representative Core/Non-core"<<endl;
        for(i=0;i<=nrow-2;i++)
        {
          out_cluster<<i<<" "<<cluster[i].status<<"/"<<cluster[i].rep<<"/"<<cluster[i].core<<endl; 
        }
        out_cluster<<i<<" "<<cluster[i].status<<"/"<<cluster[i].rep<<"/"<<cluster[i].core;
        out_cluster.close();
 
       cout<<"Final Clusters:\n";
       for(int i=0;i<=nrow-1;i++)
       {
             cout<<i<<"] ";
             cout<<cluster[i].status<<"/";
             cout<<cluster[i].rep<<"/";
             cout<<cluster[i].core<<endl;

       }

       cout<<"Clusters formed in: "<<(t2_cluster-t1_cluster);

       cout<<"\nNoise points:\n";
       double t1_noise=(double)clock()/CLOCKS_PER_SEC;      
       int noise=0;
       for(int i=0;i<=nrow-1;i++)
       {
          if(cluster[i].core==0 && cluster[i].rep==-1)
           {
             cout<<i<<" ";
             noise++;    
           }
       }
       double t2_noise=(double)clock()/CLOCKS_PER_SEC;

       //TIME ELAPSED

       cout<<endl<<"\nAlgorithm details:";
       cout<<endl<<"Core points: "<<core.size();
       cout<<endl<<"Non-Core points: "<<non_core.size(); 
       cout<<endl<<"Noise points: "<<noise; 
       cout<<endl<<"Clustered non-core points: "<<non_core.size()-noise; 
       cout<<endl<<"Data created in: "<<(t2_data-t1_data);
       cout<<endl<<"NN lists calculated in: "<<(t2_NN-t1_NN);
       cout<<endl<<"Similarity matrix calculated in: "<<(t2_sim-t1_sim);
       cout<<endl<<"Core/Non-core pts. calculated in: "<<(t2_core-t1_core);
       cout<<endl<<"Clusters formed in: "<<(t2_cluster-t1_cluster);
       cout<<endl<<"Noise pts. calculated in: "<<(t2_noise-t1_noise);   
  
       double t1_write=(double)clock()/CLOCKS_PER_SEC;
       write_binary_values(input_file_bin);
       double t2_write=(double)clock()/CLOCKS_PER_SEC;
       cout<<endl<<"Writing values...: "<<(t2_write-t1_write);

       Time = (t2_data-t1_data)+(t2_NN-t1_NN)+(t2_sim-t1_sim)+(t2_core-t1_core)+(t2_cluster-t1_cluster)+(t2_noise-t1_noise)+(t2_write-t1_write); 

       cout<<endl<<"Total time elapsed: "<<(t2_data-t1_data)+(t2_NN-t1_NN)+(t2_sim-t1_sim)+(t2_core-t1_core)+(t2_cluster-t1_cluster)+(t2_noise-t1_noise)+(t2_write-t1_write)<<endl;

        write_values(input_file_string);


       //Time taken
       ofstream  time_ni("Time_Ni",ios::app);
       time_ni<<Time<<endl;
       time_ni.close();

        //Memory Footprint
		     ofstream mem_ni("Mem_Ni",ios::app);
		     void process_mem_usage(double&, double&);
		     double vm, rss;
		     process_mem_usage(vm, rss);
		     mem_ni << vm <<" "<< rss << endl;

  return 0;
}




//Func. to calculate the similarity betwn pts. i and j
int link_strength(int a, int b)
{

  int n1,n2;
  n1=NN_list[a].size();
  n2=NN_list[b].size();

  int i=0,j=0,common=0;
	   while (i < n1 && j < n2) 
           {
             if(NN_list[a][i] < NN_list[b][j])
             {		   
                i++;
                continue;
             }

             if(NN_list[b][j] < NN_list[a][i])
             {		   
                j++;
                continue;
             }

             if(NN_list[a][i] == NN_list[b][j])
             {		   
                common++;
                i++;
                j++;
                continue;
             }


	   }  

  return common;
}


// WRITE values to ascii file
void write_values(string input_file_string)
{
   //Dump the KNN value
   ofstream out_knn_global("KNN_global");
   if(!out_knn_global)
    {
      cout<<"Cannot open the KNN_global file\n"<<endl;
      exit(0);
    }

   out_knn_global<<KNN;
   out_knn_global.close();

   //Dump the SNN value
   ofstream out_snn_global("SNN_global");
   if(!out_snn_global)
    {
      cout<<"Cannot open the SNN_global file\n"<<endl;
      exit(0);
    }

   out_snn_global<<SNN;
   out_snn_global.close();

   //Dump the DENSITY value
   ofstream out_density_global("DENSITY_global");
   if(!out_density_global)
    {
      cout<<"Cannot open the DENSITY_global file\n"<<endl;
      exit(0);
    }

   out_density_global<<DENSITY;
   out_density_global.close();

   
   //Dump the data_matrix
   ofstream out_data_matrix_global("data_matrix_global");
   if(!out_data_matrix_global)
    {
      cout<<"Cannot open the data_matrix_global file\n"<<endl;
      exit(0);
    }

    ifstream in(input_file_string.c_str());
    string str,word;
	  getline(in,str);
          out_data_matrix_global<<str<<endl;
	  char *ptr;
	  for(int i=0;i<nrow-1;i++)
	   {	      
		 getline(in,str);
		 out_data_matrix_global<<str<<endl;		 

	   }
          getline(in,str);
	  out_data_matrix_global<<str;
          out_data_matrix_global.close();
          in.close();


          //Dump NN_matrix values

	    ofstream out_nn_matrix_global("NN_matrix_global");
	    if(!out_nn_matrix_global)
	    {
	      cout<<"Cannot open the NN_matrix_global file\n"<<endl;
	      exit(0);
	    }

         int i,j;
         for(i=0;i<=nrow-1;i++)
	    {
		      for(j=0;j<KNN-1;j++)
		      {
		       out_nn_matrix_global<<NN_matrix[i][j].first<<"/";
		       out_nn_matrix_global<<NN_matrix[i][j].second<<" ";

		      }
		      
		       out_nn_matrix_global<<NN_matrix[i][j].first<<"/";
		       out_nn_matrix_global<<NN_matrix[i][j].second;

		       if(i<nrow)
		       out_nn_matrix_global<<endl;

	    }

    out_nn_matrix_global.close();

     //Dump the sparse_similarity_matrix values
	    ofstream out_sim_matrix_global("similarity_matrix_global");
	    if(!out_sim_matrix_global)
	    {
	      cout<<"Cannot open the similarity_matrix_global file\n"<<endl;
	      exit(0);
	    }

	  for(int i=0;i<=nrow-1;i++)
       		{
		  
		  for(int j=0;j<similarity_matrix[i].size();j++)
		  {
		     out_sim_matrix_global<<similarity_matrix[i][j].first<<"/";
		     out_sim_matrix_global<<similarity_matrix[i][j].second<<" ";
		  }
	         if(i<nrow-1)
		 out_sim_matrix_global<<endl;
       		} 


	    out_sim_matrix_global.close();

           //Dump core point values
	    ofstream out_core_global("core_global");
	    if(!out_core_global)
	    {
	      cout<<"Cannot open the core_global file\n"<<endl;
	      exit(0);
	    }

            if(core.size() == 0)out_core_global<<"";

            if(core.size() > 0){

	    for(i=0;i<core.size()-1;i++)
	     {
               out_core_global<<core[i]<<endl;	      
	     }
	     out_core_global<<core[i];	     
	     out_core_global.close();

            }

             //Dump non-core point values
	    ofstream out_non_core_global("non_core_global");
	    if(!out_non_core_global)
	    {
	      cout<<"Cannot open the non_core_global file\n"<<endl;
	      exit(0);
	    }

            if(non_core.size() == 0)out_non_core_global<<"";

            if(non_core.size() > 0){

		    for(i=0;i<non_core.size()-1;i++)
		     {
		       out_non_core_global<<non_core[i]<<endl;	      
		     }
		     out_non_core_global<<non_core[i];	     
		     out_non_core_global.close();

            }


             //Dump cluster values
	     ofstream out_cluster_global("Cluster_global");
	     if(!out_cluster_global)
	     {
	      cout<<"Cannot open the out_cluster_global file\n"<<endl;
	      exit(0);
	     }
             int p;
	     for(p=0;p<nrow-1;p++)
		 {
                   out_cluster_global<<p<<"]";
		   out_cluster_global<<cluster[p].status<<"/";
		   out_cluster_global<<cluster[p].rep<<"/";
		   out_cluster_global<<cluster[p].core;
		   out_cluster_global<<endl;

		 }
              out_cluster_global<<p<<"]";
	      out_cluster_global<<cluster[p].status<<"/";
	      out_cluster_global<<cluster[p].rep<<"/";
	      out_cluster_global<<cluster[p].core;

	      //out_cluster_global<<endl<<"Cluster status/Cluster Rep./Core-NonCore";
	      out_cluster_global.close();

	      ofstream out_nrow_ncol("Row_Column");
	      out_nrow_ncol<<nrow<<" "<<ncol;
	      out_nrow_ncol.close();
 

}//end func


//WRITE values to binary file
void write_binary_values(string input_file_bin)
{
   //Dump the KNN value
   ofstream knn_binary;
   knn_binary.open("KNN_global.bin",ios::out|ios::binary);
   if(!knn_binary)
    {
      cout<<"Cannot open the KNN_global.bin file"<<endl;
      exit(0);
    }

   knn_binary.write((char*)&KNN,sizeof(KNN)); 
   knn_binary.close(); 

   /*int knn;
   ifstream knn_bin("KNN_global.bin",ios::in|ios::binary);
   knn_bin.read((char*)&knn,sizeof(knn));
   cout<<endl<<knn;
   knn_bin.close();*/

   //Dump the KNN value 
   ofstream snn_binary;
   snn_binary.open("SNN_global.bin",ios::out|ios::binary);
   if(!snn_binary)
    {
      cout<<"Cannot open the SNN_global.bin file"<<endl;
      exit(0);
    }

   snn_binary.write((char*)&SNN,sizeof(SNN));
   snn_binary.close();
   
   /*int snn;
   ifstream snn_bin("SNN_global.bin",ios::in|ios::binary);
   snn_bin.read((char*)&snn,sizeof(snn));
   cout<<endl<<snn;
   snn_bin.close();*/
 

   //Dump the DENSITY value
   ofstream density_binary;
   density_binary.open("DENSITY_global.bin",ios::out|ios::binary);
   if(!density_binary)
    {
      cout<<"Cannot open the DENSITY_global.bin file\n"<<endl;
      exit(0);
    }

   density_binary.write((char*)&DENSITY,sizeof(DENSITY));
   density_binary.close();

   /*int density;
   ifstream density_bin("DENSITY_global.bin",ios::in|ios::binary);
   density_bin.read((char*)&density,sizeof(density));
   cout<<endl<<density;
   density_bin.close();*/
   

   //Dump the data_matrix
   ofstream data_matrix_binary;
   data_matrix_binary.open("data_matrix_global.bin",ios::out|ios::binary);
   if(!data_matrix_binary)
    {
      cout<<"Cannot open the data_matrix_global.bin file\n"<<endl;
      exit(0);
    }

   ifstream in_data;
   in_data.open(input_file_bin.c_str(),ios::in|ios::binary);
   if(!in_data)
   {
       cout<<"Cannot open the "<<input_file_bin <<" file\n"<<endl;
       exit(0);
   }

   int nrow1,ncol1;
   //Read rows and columns
   in_data.read((char*)&nrow1,sizeof(nrow1));
   in_data.read((char*)&ncol1,sizeof(ncol1));
   //cout<<nrow1<<ncol1;

   //Write the rows and columns values
   data_matrix_binary.write((char*)&nrow1,sizeof(nrow1));
   data_matrix_binary.write((char*)&ncol1,sizeof(ncol1));

   long double temp;
   //Read and Write other values
   for(int i=0;i<=nrow1-1;i++)
    {
       for(int j=0;j<=ncol1-1;j++)
       {
	 in_data.read((char*)&temp,sizeof(long double));
	 data_matrix_binary.write((char*)&temp,sizeof(temp));
       }
    }

   in_data.close();data_matrix_binary.close();

   /*int rc;
   ifstream data("data_matrix_global.bin",ios::in|ios::binary);
   data.read((char*)&rc,sizeof(int));
   data.read((char*)&rc,sizeof(int));
   for(int i=0;i<=nrow-1;i++)
    {
       for(int j=0;j<=ncol-1;j++)
       {
	 data.read((char*)&temp,sizeof(long double));
	 cout<<temp<<" ";
       }
      cout<<endl;
    }
    data.close()*/


    //Dump NN_matrix values
     ofstream nn_binary("NN_matrix_global.bin",ios::out|ios::binary);
	    if(!nn_binary)
	    {
	      cout<<"Cannot open the NN_matrix_global.bin file\n"<<endl;
	      exit(0);
	    }

     //cout<<nrow<<ncol; 
     pair<long double,int>nn_pair;
     
     for(int i=0;i<=nrow-1;i++)
     {
        for(int j=0;j<=KNN-1;j++)
        {
           nn_pair.first = NN_matrix[i][j].first;         
           nn_pair.second = NN_matrix[i][j].second;

           nn_binary.write((char*)&nn_pair,sizeof(nn_pair));
        }
     }

     nn_binary.close();

     /*pair<long double,int>nn_temp;
     ifstream nn("NN_matrix_global.bin",ios::in|ios::binary);
     for(int i=0;i<=nrow-1;i++)
     {
        for(int j=0;j<=KNN-1;j++)
        {
           nn.read((char*)&nn_temp,sizeof(nn_temp));
           cout<<nn_temp.first<<"/"<<nn_temp.second<<" ";
        }
       cout<<endl;
     }
      nn.close();*/


     //Dump the sparse_similarity_matrix values
	    ofstream sim_matrix_binary("similarity_matrix_global.bin",ios::out|ios::binary);
	    if(!sim_matrix_binary)
	    {
	      cout<<"Cannot open the similarity_matrix_global.bin file\n"<<endl;
	      exit(0);
	    }

   
         pair<int,int>sim_pair;
         int row_size;
         for(int i=0;i<=nrow-1;i++)
       		{
			  row_size=similarity_matrix[i].size();
                          sim_matrix_binary.write((char*)&row_size,sizeof(row_size));
			  for(int j=0;j<similarity_matrix[i].size();j++)
			  {
			     sim_pair.first = similarity_matrix[i][j].first;
			     sim_pair.second = similarity_matrix[i][j].second;

		             sim_matrix_binary.write((char*)&sim_pair,sizeof(sim_pair));
			  }
	         
       		} 

           sim_matrix_binary.close();

           /*pair<int,int>sim_temp;
	     ifstream sim("similarity_matrix_global.bin",ios::in|ios::binary);
	     for(int i=0;i<=nrow-1;i++)
	     {
                sim.read((char*)&row_size,sizeof(row_size));
                cout<<"["<<row_size<<"]"; 
		for(int j=0;j<similarity_matrix[i].size();j++)
		{
		   sim.read((char*)&sim_temp,sizeof(sim_temp));
		   cout<<sim_temp.first<<"/"<<sim_temp.second<<" ";
		}
	       cout<<endl;
	     }
              sim.close();*/

          //Dump core point values
           ofstream core_binary("core_global.bin",ios::out|ios::binary);
	    if(!core_binary)
	    {
	      cout<<"Cannot open the core_global.bin file\n"<<endl;
	      exit(0);
	    }
            int a;
            if(core.size() == 0)core_binary.write((char*)&a,0);
           
            if(core.size() > 0){
		    for(int i=0;i<core.size();i++)
		     {
		       core_binary.write((char*)&core[i],sizeof(core[i]));	      
		     }
	      }
            core_binary.close();

            /*ifstream core_bin("core_global.bin",ios::in|ios::binary);
            for(int i=0;i<core.size();i++)
            {
               core_bin.read((char*)&a,sizeof(a));
               cout<<a<<" ";
            }
             core_bin.close()*/

           //Dump non-core point values
           ofstream non_core_binary("non_core_global.bin",ios::out|ios::binary);
	    if(!core_binary)
	    {
	      cout<<"Cannot open the non_core_global.bin file\n"<<endl;
	      exit(0);
	    }

            if(non_core.size() == 0)non_core_binary.write((char*)&a,0);
            if(non_core.size() > 0){
		    for(int i=0;i<non_core.size();i++)
		     {
		       non_core_binary.write((char*)&non_core[i],sizeof(non_core[i]));	      
		     }
               }
	     
           non_core_binary.close();

           /*ifstream non_core_bin("non_core_global.bin",ios::in|ios::binary);
            for(int i=0;i<non_core.size();i++)
            {
               non_core_bin.read((char*)&a,sizeof(a));
               cout<<a<<" ";
            }
             non_core_bin.close();*/            

           //Dump cluster values
	     ofstream cluster_binary("Cluster_global.bin",ios::out|ios::binary);
	     if(!cluster_binary)
	     {
	      cout<<"Cannot open the Cluster_global.bin file\n"<<endl;
	      exit(0);
	     }

	     for(int i=0;i<=nrow-1;i++)
		 {
		    cluster_binary.write((char*)&cluster[i],sizeof(cluster[i]));

		 }

	     cluster_binary.close();

             /*ifstream cluster("Cluster_global.bin",ios::out|ios::binary);
             Cluster temp_cluster;
             for(int i=0;i<=nrow-1;i++)
             {
               cluster.read((char*)&temp_cluster,sizeof(temp_cluster));
               cout<<temp_cluster.status<<"/"<<temp_cluster.rep<<"/"<<temp_cluster.core<<endl;
             }*/

              ofstream rowcol_binary("Row_Column.bin",ios::out|ios::binary);
	      rowcol_binary.write((char*)&nrow,sizeof(nrow));
              rowcol_binary.write((char*)&ncol,sizeof(ncol));
	      rowcol_binary.close();

             /*ifstream rowcol("Row_Column.bin",ios::in|ios::binary);
             rowcol.read((char*)&nrow,sizeof(nrow));
             rowcol.read((char*)&ncol,sizeof(ncol));
             cout<<nrow<<ncol;
	     rowcol.close();*/

}//end func

void link_cluster(int rp,int rq)
{
   if(rp<rq)
   {
      for(int i=0;i<=nrow-1;i++)  
      {
        if(cluster[i].rep==rq)
         cluster[i].rep=rp;
      }
   }

   if(rq<rp)
   {
      for(int i=0;i<=nrow-1;i++)  
      {
        if(cluster[i].rep==rp)
         cluster[i].rep=rq;
      }
   }
}//end func

//MEMORY FOOTPRINT
     void process_mem_usage(double& vm_usage, double& resident_set)
	{
	   using std::ios_base;
	   using std::ifstream;
	   using std::string;

	   vm_usage     = 0.0;
	   resident_set = 0.0;

	   // 'file' stat seems to give the most reliable results
	   //
	   ifstream stat_stream("/proc/self/stat",ios_base::in);

	   // dummy vars for leading entries in stat that we don't care about
	   //
	   string pid, comm, state, ppid, pgrp, session, tty_nr;
	   string tpgid, flags, minflt, cminflt, majflt, cmajflt;
	   string utime, stime, cutime, cstime, priority, nice;
	   string O, itrealvalue, starttime;

	   // the two fields we want
	   //
	   unsigned long vsize;
	   long rss;

	   stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
		       >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
		       >> utime >> stime >> cutime >> cstime >> priority >> nice
		       >> O >> itrealvalue >> starttime >> vsize >> rss; // don't care about the rest

	   stat_stream.close();

	   long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
	   vm_usage     = vsize / 1024.0;
	   resident_set = rss * page_size_kb;
	}




























