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
#include <malloc.h>
using namespace std;

void write_values(string);
void write_binary_values();
int link_strength(int,int);
void BFS_snn(int);
void BFS_split(int,int);
int snn_conn(int,int);
void link_cluster(int,int);
void non_core_alloc(int);
void core_alloc(int);

vector< vector<long double> >data_matrix;
vector< vector<long double> >data_matrix_inc;
vector< vector< pair<long double,int> > >NN_matrix; 
vector< vector<int> >NN_list;
vector< vector< pair<int,int> > >similarity_matrix;
vector< vector< pair<int,int> > >similarity_matrix_inc;

vector<int>core;
vector<int>non_core;
vector<int>core_inc; 
vector<int>non_core_inc;
vector<int>noise; 
vector<int>noise_inc;

vector<int>type1;
vector<int>type2;
vector<int>type3;
vector<int>visit;

string init_data,inc_data;
int KNN,SNN,DENSITY;
int nrow1,nrow2,nrow;
int ncol,snn_reach=0;

typedef struct Clust{

     public: 
      int status;
      int rep;
      int core;
}Cluster;
vector<Cluster>cluster;
vector<Cluster>cluster_inc;
vector<int>temp_cluster;

int main(int argc, char* argv[])
{

                                    //*******Fetching the Algorithm parameters*********//

  double t1_param=(double)clock()/CLOCKS_PER_SEC;
  ifstream knn("KNN_global.bin",ios::in|ios::binary);
  knn.read((char*)&KNN,sizeof(KNN));

  ifstream snn("SNN_global.bin",ios::in|ios::binary);
  snn.read((char*)&SNN,sizeof(SNN));

  ifstream density("DENSITY_global.bin",ios::in|ios::binary);
  density.read((char*)&DENSITY,sizeof(DENSITY));

  knn.close();snn.close();density.close();
  double t2_param=(double)clock()/CLOCKS_PER_SEC;
  cout<<"Algorithm parameters set:(KNN size="<<KNN<<" ; SNN threshold="<<SNN<<" ; DENSITY threshold="<<DENSITY<<")\n";
  cout<<(t2_param-t1_param);

                                
                                   //********Fetching the Original data file*******//
  
          

  double t1_data_fetch=(double)clock()/CLOCKS_PER_SEC; 
  string input_file = argv[1];
  string input_file_string = input_file;
  string input_file_bin = input_file+".bin";
  init_data = input_file_bin;

  ifstream in(input_file_bin.c_str()); 
  if(!in)
  {
    cout<<"Cannot open the input file: "<<input_file_bin<<endl;
    exit(0);
  } 

  //Read the nrow,ncol values from input binary file
    
    in.read((char*)&nrow1,sizeof(int));
    in.read((char*)&ncol,sizeof(int));
    cout<<"\nOriginal file:"<<input_file_bin<<"\n";
    cout<<"Rows:"<<nrow1;
    cout<<" Columns:"<<ncol<<endl;

    data_matrix.resize(nrow1);
    for(int i=0;i<=nrow1-1;i++)
      data_matrix[i].resize(ncol);
 
    //Read the rest of the values from the input binary file
	  long double temp;
	  for(int i=0;i<=nrow1-1;i++)
	    {
	       for(int j=0;j<=ncol-1;j++)
	       {
		 in.read((char*)&temp,sizeof(long double));
		 data_matrix[i][j] = temp;
	       }
	    }
     double t2_data_fetch=(double)clock()/CLOCKS_PER_SEC;     
	   /*cout<<"The Original data matrix is:\n";
	   for(int i=0;i<=nrow1-1;i++)
	   {
	      for(int j=0;j<=ncol-1;j++)
	      {
		 cout<<data_matrix[i][j]<<" ";
	      }
	    
	     cout<<endl;
	   }*/
      cout<<(t2_data_fetch-t1_data_fetch);
	 
	   in.close();

                            
                                             //********Fetching the Incremental data file*******//  


  double t1_inc_data=(double)clock()/CLOCKS_PER_SEC;
  string inc_file = argv[2];
  string inc_file_string = inc_file;
  string inc_file_bin = inc_file+".bin";
  inc_data = inc_file_bin; 

  ifstream inc(inc_file_bin.c_str()); 
  if(!inc)
  {
    cout<<"Cannot open the incremental file: "<<inc_file_bin<<endl;
    exit(0);
  }

  //Read the nrow,ncol values from input incremental binary file
  
    inc.read((char*)&nrow2,sizeof(int));
    inc.read((char*)&ncol,sizeof(int));
 
  cout<<"\nIncremental file:"<<inc_file_bin<<"\n";
  cout<<"Rows:"<<nrow2;
  cout<<" Columns:"<<ncol<<endl;  

  data_matrix_inc.resize(nrow2); 
  for(int i=0;i<=nrow2-1;i++)
      data_matrix_inc[i].resize(ncol);  
                                              
  //Read the rest of the values from the input incremental binary file
  for(int i=0;i<=nrow2-1;i++)
    {
       for(int j=0;j<=ncol-1;j++)
       {
	 inc.read((char*)&temp,sizeof(long double));
	 data_matrix_inc[i][j] = temp;
       }
    }
   double t2_inc_data=(double)clock()/CLOCKS_PER_SEC;
   /*cout<<"The incremental data matrix is:\n";
   for(int i=0;i<=nrow2-1;i++)
   {
      for(int j=0;j<=ncol-1;j++)
      {
         cout<<data_matrix_inc[i][j]<<" ";
      }
    
     cout<<endl;
   }*/
   cout<<(t2_inc_data-t1_inc_data);
   inc.close();

         
                                            //*******Fetching the original NN_matrix********//

   double t1_NN_fetch=(double)clock()/CLOCKS_PER_SEC;
   NN_matrix.resize(nrow1);
    for(int i=0;i<=nrow1-1;i++)
      NN_matrix[i].resize(KNN);

   ifstream nn_matrix;
   if(init_data=="data_matrix_global.bin")
    nn_matrix.open("NN_matrix_global.bin",ios::in|ios::binary);
   else
    nn_matrix.open("NN_matrix_inc.bin",ios::in|ios::binary);

   if(!nn_matrix)
   {
     cout<<"Cannot open the NN_matrix_global.bin"<<endl;
     exit(0);
   }
   
   pair<long double,int>nn_temp;
   for(int i=0;i<=nrow1-1;i++)
   {
     for(int j=0;j<=KNN-1;j++)
     {
        nn_matrix.read((char*)&nn_temp,sizeof(nn_temp));
        NN_matrix[i][j].first = nn_temp.first;
        NN_matrix[i][j].second = nn_temp.second;
     }

   }
   nn_matrix.close();
   double t2_NN_fetch=(double)clock()/CLOCKS_PER_SEC;
   cout<<"\nThe Original NN matrix is:\n";
    /*for(int i=0;i<=nrow1-1;i++)
          {
		cout<<i<<"] ";    
		    for(int j=0;j<=KNN-1;j++)
		    {
		      cout<<NN_matrix[i][j].first<<"/"<<NN_matrix[i][j].second<<" ";
		      
		    }  

              cout<<endl;

           }*/ 
               
    cout<<(t2_NN_fetch-t1_NN_fetch);
                     
                                           //*******Fetching the original similarity_matrix********//


     double t1_sim_fetch=(double)clock()/CLOCKS_PER_SEC; 
       similarity_matrix.resize(nrow1); 
       similarity_matrix_inc.resize(nrow1);                 
         ifstream sim_matrix;
         if(init_data=="data_matrix_global.bin")
         sim_matrix.open("similarity_matrix_global.bin",ios::in|ios::binary);
         else
         sim_matrix.open("similarity_matrix_inc.bin",ios::in|ios::binary);
         if(!sim_matrix)
         {
            cout<<"Cannot open the NN_matrix_global.bin"<<endl;
            exit(0);

         } 
        
        pair<int,int>sim_temp;
         int row_size;
                 for(int i=0;i<=nrow1-1;i++)
                 {
                    sim_matrix.read((char*)&row_size,sizeof(row_size));
                    if(row_size == 0) continue;

                    if(row_size > 0)
                    {
                      for(int j=0;j<row_size;j++)
                       {
                          sim_matrix.read((char*)&sim_temp,sizeof(sim_temp));
                          similarity_matrix[i].push_back(sim_temp);
                          //similarity_matrix_inc[i].push_back(sim_temp);
                       }
                    }

                 }
         double t2_sim_fetch=(double)clock()/CLOCKS_PER_SEC;
         cout<<"\nThe Original Similarity Matrix is:\n";
		  /*    for(int i=0;i<=nrow1-1;i++)
		       {
				  cout<<i<<"] ";
				  for(int j=0;j<similarity_matrix[i].size();j++)
				  {
				     cout<<similarity_matrix[i][j].first<<"/";
				     cout<<similarity_matrix[i][j].second<<" ";
				  }
			       
				 cout<<endl;
		       } */
         sim_matrix.close();
         if(init_data=="data_matrix_global.bin")
         sim_matrix.open("similarity_matrix_global.bin",ios::in|ios::binary);
         else
         sim_matrix.open("similarity_matrix_inc.bin",ios::in|ios::binary);
         if(!sim_matrix)
         {
            cout<<"Cannot open the NN_matrix_global.bin"<<endl;
            exit(0);

         }     
     
              for(int i=0;i<=nrow1-1;i++)
                 {
                    sim_matrix.read((char*)&row_size,sizeof(row_size));
                    if(row_size == 0) continue;

                    if(row_size > 0)
                    {
                      for(int j=0;j<row_size;j++)
                       {
                          sim_matrix.read((char*)&sim_temp,sizeof(sim_temp));
                          similarity_matrix_inc[i].push_back(sim_temp);
                       }
                    }

                 }

            sim_matrix.close();
            cout<<(t2_sim_fetch-t1_sim_fetch);  

          
                                           //*********Fetching the Core/Non-core pts.**********//


                double t1_core_fetch=(double)clock()/CLOCKS_PER_SEC; 
                int ncr;
                ifstream core_global;
                if(init_data=="data_matrix_global.bin") 
                core_global.open("core_global.bin",ios::in|ios::binary);
                else
                core_global.open("core_inc.bin",ios::in|ios::binary);

                if(!core_global)
                {
                   cout<<"Cannot open the core_global.bin file"<<endl;
                   exit(0);
                }
              
               while(1)
               {
                  core_global.read((char*)&ncr,sizeof(ncr));
                  if(core_global.gcount() == 0)
                   break;
                  core.push_back(ncr);
                  core_inc.push_back(ncr);
               }
               core_global.close(); 
               double t2_core_fetch=(double)clock()/CLOCKS_PER_SEC;
               cout<<"\nOriginal Core points:\n";
	       /*for(int i=0;i<core.size();i++)
	       {
		 cout<<core[i]<<" ";

	       }*/
               cout<<(t2_core_fetch-t1_core_fetch);

               double t1_non_core_fetch=(double)clock()/CLOCKS_PER_SEC;
               ifstream non_core_global;
               if(init_data=="data_matrix_global.bin")
               non_core_global.open("non_core_global.bin",ios::in|ios::binary);
               else
               non_core_global.open("non_core_inc.bin",ios::in|ios::binary);

               if(!non_core_global)
                {
                   cout<<"Cannot open the non_core_global.bin file"<<endl;
                   exit(0);
                }
               
               while(1)
               {
                  non_core_global.read((char*)&ncr,sizeof(ncr));
                  if(non_core_global.gcount() == 0)
                   break;
                  non_core.push_back(ncr);
                  non_core_inc.push_back(ncr);
               }
               non_core_global.close();
               double t2_non_core_fetch=(double)clock()/CLOCKS_PER_SEC;  
               cout<<"\nOriginal Non-Core points:\n";
	       /*for(int i=0;i<non_core.size();i++)
	       {
		 cout<<non_core[i]<<" ";

	       } */

               cout<<(t2_non_core_fetch-t1_non_core_fetch);


                                          //*******Fetching the Original CLUSTERS and Noise pts.********//


               double t1_cluster_fetch=(double)clock()/CLOCKS_PER_SEC;
               cluster.resize(nrow1);
                ifstream cluster_global;
                if(init_data=="data_matrix_global.bin")
                cluster_global.open("Cluster_global.bin",ios::in|ios::binary);
                else
                cluster_global.open("Cluster_inc.bin",ios::in|ios::binary);

                if(!cluster_global)
                {
                   cout<<"Cannot open the Cluster_global.bin file"<<endl;
                   exit(0);
                }

                //vector<Cluster>cluster_inc;
                
                cluster_inc.resize(nrow1);
                Cluster cluster_temp;
                for(int i=0;i<=nrow1-1;i++)
                { 
                   cluster_global.read((char*)&cluster_temp,sizeof(cluster_temp));
                   cluster[i]=cluster_temp;
                   //cluster_inc[i]=cluster[i]; //Assigning to new working cluster vector
                   
                }
                double t2_cluster_fetch=(double)clock()/CLOCKS_PER_SEC;
                //Working Cluster
                for(int i=0;i<=nrow1-1;i++)
                { 
                  cluster_inc[i]=cluster[i]; //Assigning to new working cluster vector                   
                }

                 cout<<"\nOriginal Clusters:\n";
		    /*  for(int i=0;i<=nrow1-1;i++)
		       {
			     cout<<i<<"] ";
			     cout<<cluster[i].status<<"/";
			     cout<<cluster[i].rep<<"/";
			     cout<<cluster[i].core<<endl;

		       }*/

                cout<<(t2_cluster_fetch-t1_cluster_fetch);

                cout<<"\nOriginal Noise points:\n"; //to track initial noise pts.
                double t1_noise_fetch=(double)clock()/CLOCKS_PER_SEC;
                for(int i=0;i<non_core.size();i++)
                {
                   if(cluster[non_core[i]].rep==-1)
                   noise.push_back(non_core[i]);
                   
                }
                double t2_noise_fetch=(double)clock()/CLOCKS_PER_SEC;
                /*for(int i=0;i<noise.size();i++)
                 cout<<noise[i]<<" ";*/
                cout<<(t2_noise_fetch-t1_noise_fetch);
 

                 double t_fetch=(t2_param-t1_param)+(t2_data_fetch-t1_data_fetch)+(t2_inc_data-t1_inc_data)+(t2_NN_fetch-t1_NN_fetch)+(t2_sim_fetch-t1_sim_fetch)+(t2_core_fetch-t1_core_fetch)+(t2_non_core_fetch-t1_non_core_fetch)+(t2_cluster_fetch-t1_cluster_fetch)+(t2_noise_fetch-t1_noise_fetch);

                 
                 cout<<"\nTotal fetch time:"<<t_fetch;




                                                //<----------INCREMENTAL PHASE---------->//

                                            
                                  //****Creating the Original file + Incremental file to work with****// 


   cout<<endl<<"\nIncremental Phase begins...\n";
   nrow = nrow1+nrow2;
   //cout<<nrow;
   double t1_data_inc=(double)clock()/CLOCKS_PER_SEC;
   data_matrix.resize(nrow);        
   int i=nrow1;

   for(int i1=0;i1<nrow2;i1++,i++)
   {
      for(int j=0;j<ncol;j++)
      {
         data_matrix[i].push_back(data_matrix_inc[i1][j]);
      }
   
   } 
   double t2_data_inc=(double)clock()/CLOCKS_PER_SEC;
	   cout<<"The Updated data matrix is:\n";
           cout<<"Rows:"<<nrow<<" Columns:"<<ncol<<endl; 
	  /* for(int i=0;i<=nrow-1;i++)
	   {
             cout<<i<<"] ";    
	      for(int j=0;j<=ncol-1;j++)
	      {
		 cout<<data_matrix[i][j]<<" ";
	      }
	    
	     cout<<endl;
	   }*/

  cout<<(t2_data_inc-t1_data_inc);

                                           //******Updating the NN_list + Find Type1 pts.******//


           double t1_NN_inc=(double)clock()/CLOCKS_PER_SEC;        
            long double dist_ij,sum=0;
            vector< pair<long double,int> >temp_dist(nrow2);
            NN_matrix.resize(nrow);
            NN_list.resize(nrow);
            
	    for(int i=0;i<nrow1;i++)
	    {
               //cout<<i<<endl; 
                sum=0;
                for(int i1=nrow1,l=0;i1<nrow;i1++,l++)		        
                {
                   sum=0;
                   for(int j=0;j<ncol;j++)
                     sum += pow((data_matrix[i][j]-data_matrix[i1][j]),2);      

                   dist_ij=sqrt(sum); //sqrt
                   temp_dist[l].first = dist_ij;
                   temp_dist[l].second = i1;

                    if(i<=KNN-1){
                     NN_matrix[i1].push_back(pair<long double,int>(dist_ij,i));                    
                     sort(NN_matrix[i1].begin(),NN_matrix[i1].end());
                    }
                    else
                     {
                       if(dist_ij >= NN_matrix[i1][KNN-1].first);
                       else
                        {
                          NN_matrix[i1].push_back(pair<long double,int>(dist_ij,i));
                          sort(NN_matrix[i1].begin(),NN_matrix[i1].end());
                          NN_matrix[i1].pop_back();
                        } 
                     } 
                   
                }//end for

                  sort(temp_dist.begin(),temp_dist.end());
                  int k; 
                  for(k=0;k<temp_dist.size();k++)
                  {
	                  if(NN_matrix[i][KNN-1].first <= temp_dist[k].first) 
	                   break;
	                  else
                           NN_matrix[i].push_back(pair<long double,int>(temp_dist[k].first,temp_dist[k].second));                            
                          
                  }

                 if(k>0) 
                  {
                    sort(NN_matrix[i].begin(),NN_matrix[i].end());
                    //NN_matrix[i].erase(NN_matrix[i].begin()+KNN,NN_matrix[i].end());
                  }  

                 

	    }//end for 

                  
            //NN_list of the new pts.
            for(int i=nrow1;i<nrow;i++)
            { 
              sum=0; 
               for(int i1=nrow1;i1<nrow;i1++)
               {
                 if(i==i1)continue;
                  sum=0;
                   for(int j=0;j<ncol;j++)                          
                     sum += pow((data_matrix[i][j]-data_matrix[i1][j]),2);

                   dist_ij=sqrt(sum); //sqrt
                   if(dist_ij >= NN_matrix[i][KNN-1].first);
                   else
                    {
                      NN_matrix[i].push_back(pair<long double,int>(dist_ij,i1));
                      sort(NN_matrix[i].begin(),NN_matrix[i].end());
                      NN_matrix[i].pop_back();
                    }
                   
               }

              
            }//end for
           double t2_NN_inc=(double)clock()/CLOCKS_PER_SEC;
             for(int i=0;i<=nrow-1;i++)
		  {

			    for(int j=0;j<=KNN-1;j++)
			    {
			      NN_list[i].push_back(NN_matrix[i][j].second);			      
			    }

		      sort(NN_list[i].begin(),NN_list[i].end());
		   } 

         
         cout<<"\nThe Extended/Updated NN matrix is:\n";
	    /*for(int i=0;i<=nrow-1;i++)
		  {
			cout<<i<<"] ";    
			    for(int j=0;j<NN_matrix[i].size();j++)
			    {
			       cout<<NN_matrix[i][j].first<<"/"<<NN_matrix[i][j].second<<" ";			      
			    }  

                            for(int j=0;j<=KNN-1;j++)
			    {
			       cout<<NN_list[i][j]<<" ";			      
			    }  

		      cout<<endl;

		   }*/

          cout<<(t2_NN_inc-t1_NN_inc);

           

                                             //**********Labelling the Type1 points*********//
                   double t1_type1=(double)clock()/CLOCKS_PER_SEC;
                   for(int i=0;i<nrow1;i++)
                   {
                      if(NN_matrix[i].size()>KNN)
                       {
                        //if(!binary_search(type1.begin(),type1.end(),i))
                           type1.push_back(i);

                         NN_matrix[i].erase(NN_matrix[i].begin()+KNN,NN_matrix[i].end());                         
                       } 
                   }
                  double t2_type1=(double)clock()/CLOCKS_PER_SEC;

            cout<<"\nThe Updated NN matrix is:\n";
	    /* for(int i=0;i<=nrow-1;i++)
		  {
			cout<<i<<"] ";    
			    for(int j=0;j<NN_matrix[i].size();j++)
			    {
			       cout<<NN_matrix[i][j].first<<"/"<<NN_matrix[i][j].second<<" ";			      
			    }  

                            for(int j=0;j<=KNN-1;j++)
			    {
			       cout<<NN_list[i][j]<<" ";			      
			    }  

		      cout<<endl;

		   }*/
 

                   cout<<"\nType 1 affected points:("<<((double)type1.size()/nrow)*100<<"%)"<<endl;;
		          /* for(int i=0;i<type1.size();i++)
		             cout<<type1[i]<<" ";
                             cout<<endl;*/

                cout<<(t2_type1-t1_type1);

                               //**********************Updating the Similarity Matrix********************//

                  //Updating the similarity rows of new pts.
                  double t1_sim_inc=(double)clock()/CLOCKS_PER_SEC;
                  similarity_matrix_inc.resize(nrow); 
                  int common; 
                  for(int i=nrow1;i<nrow;i++)
                  {
                       for(int j=0;j<NN_list[i].size();j++)
                       { 
		           int k = NN_list[i][j];
                           //cout<<k<<" ";
                           if(binary_search(NN_list[k].begin(),NN_list[k].end(),i))
                           {
                              common = link_strength(i,k);
                              if(common >= SNN)
                              {
                                 similarity_matrix_inc[i].push_back(pair<int,int>(k,common));
                              }

                           }
  
                       }//end for


                  }//end for     

                   cout<<"\nThe Updated Similarity Matrix(new pts.) is:\n";
		     /* for(int i=0;i<=nrow-1;i++)
		       {
				  cout<<i<<"] ";
				  for(int j=0;j<similarity_matrix_inc[i].size();j++)
				  {
				     cout<<similarity_matrix_inc[i][j].first<<"/";
				     cout<<similarity_matrix_inc[i][j].second<<" ";
				  }
			       
				 cout<<endl;
		       }*/ 

                   //Updating the similarity rows of the type1 affected pts.
                   for(int i=0;i<type1.size();i++)
                   {
                       int t1=type1[i];
                       similarity_matrix_inc[t1].erase(similarity_matrix_inc[t1].begin(),similarity_matrix_inc[t1].end());
                       //cout<<t1;
                       for(int j=0;j<KNN;j++)       
                       {
                          int k = NN_list[t1][j];  
                          if(binary_search(NN_list[k].begin(),NN_list[k].end(),t1))
                          {
                             common = link_strength(t1,k);
                             if(common >= SNN)
                             {
                                 similarity_matrix_inc[t1].push_back(pair<int,int>(k,common));
                             }
                          }
                       }//end for
   
                   }//end for 

                    cout<<"\nThe Updated Similarity Matrix(type1 pts.) is:\n";
		     /* for(int i=0;i<=nrow-1;i++)
		       {
				  cout<<i<<"] ";
				  for(int j=0;j<similarity_matrix_inc[i].size();j++)
				  {
				     cout<<similarity_matrix_inc[i][j].first<<"/";
				     cout<<similarity_matrix_inc[i][j].second<<" ";
				  }
			       
				 cout<<endl;
		       }*/ 

                    //Identifying the type 2 pts. from type1 pts. along with possible split pair
                    vector<int>s1,s2;
                    vector< pair<int,int> >split_pair; 
                    vector<int>split_type;

                    for(int i=0;i<type1.size();i++) 
                    {
                        int t1=type1[i];
                        for(int j=0;j<similarity_matrix[t1].size();j++)
                         s1.push_back(similarity_matrix[t1][j].first); //clear s1 later

                        for(int j=0;j<similarity_matrix_inc[t1].size();j++)
                         s2.push_back(similarity_matrix_inc[t1][j].first); //clear s2 later


                        //Compare s1 and s2 to find altered/missing links and type2 pts.
                        for(int j1=0;j1<s1.size();j1++)
                        {
                           int k1 = s1[j1];

	                   if(binary_search(s2.begin(),s2.end(),k1))//found
	                   {
	                      //cout<<k1<<"found";                              
	                      for(int j2=0;j2<s2.size();j2++)
	                      {
	                         if(s2[j2]==k1)
	                          {
	                             
	                            if(!binary_search(type1.begin(),type1.end(),k1))//k1 is non-type1 pt.
	                             {
	                                
	                                 
	                                     if(!binary_search(type2.begin(),type2.end(),k1))
	                                       type2.push_back(k1); //type2 pt.                                  

	                                  

	                             }

	                          }//end if

	                       }//end for

	                    }// end if

                            if(!binary_search(s2.begin(),s2.end(),k1))// not found  
                            {
                                                           

                              if(!binary_search(type1.begin(),type1.end(),k1))// k1 is non-type1 pt.
                              {
                                 //degraded rank-removed link
                                  if(!binary_search(type2.begin(),type2.end(),k1))
                                    type2.push_back(k1);                               

                              }

                            }//end if


                          }//end for

                          //Clear s1,s2

                          s1.erase(s1.begin(),s1.end());
                          s2.erase(s2.begin(),s2.end());



                    }//end for

                    //Labelling Type2 points.
                    cout<<"\nType 2 affected points:("<<((double)type2.size()/nrow)*100<<"%)"<<endl;;
		       /*      for(int i=0;i<type2.size();i++)
		             cout<<type2[i]<<" ";
                             cout<<endl;*/


                   //Updating similarity rows of type2 pts.
                   for(int i=0;i<type2.size();i++) 
                   {
                      int t2=type2[i];
                       similarity_matrix_inc[t2].erase(similarity_matrix_inc[t2].begin(),similarity_matrix_inc[t2].end());
                       //cout<<t1;
                       for(int j=0;j<KNN;j++)       
                       {
                          int k = NN_list[t2][j];  
                          if(binary_search(NN_list[k].begin(),NN_list[k].end(),t2))
                          {
                             common = link_strength(t2,k);
                             if(common >= SNN)
                             {
                                 similarity_matrix_inc[t2].push_back(pair<int,int>(k,common));
                             }
                          }
                       }//end for
                   }

                   double t2_sim_inc=(double)clock()/CLOCKS_PER_SEC; 

                     cout<<"\nThe Updated Similarity Matrix is:\n";
		     /*for(int i=0;i<=nrow-1;i++)
		       {
				  cout<<i<<"] ";
				  for(int j=0;j<similarity_matrix_inc[i].size();j++)
				  {
				     cout<<similarity_matrix_inc[i][j].first<<"/";
				     cout<<similarity_matrix_inc[i][j].second<<" ";
				  }
			       
				 cout<<endl;
		       }*/


                     cout<<(t2_sim_inc-t1_sim_inc);        


                                           //********Labelling Type3 points********//

         
                  
                                  
                                                //*****Split Pair List*****//


                                  
                                       //*********Core/Non-Core pt. detection********//
                
               //New pts. initial core/non-core status
               core_inc.erase(core_inc.begin(),core_inc.end());
               non_core_inc.erase(non_core_inc.begin(),non_core_inc.end());
               cluster_inc.resize(nrow);

               double t1_core_inc=(double)clock()/CLOCKS_PER_SEC;              
               for(int i=0;i<type1.size();i++)
               { 
                  int t1=type1[i];
                  if(similarity_matrix_inc[t1].size()>DENSITY)
                  {
                      cluster_inc[t1].core=1;                      
                  }
                  if(similarity_matrix_inc[t1].size()<=DENSITY)
                  {
                     cluster_inc[t1].core=0;
                  }
               }
               
               for(int i=0;i<type2.size();i++)
               { 
                  int t2=type2[i];
                  if(similarity_matrix_inc[t2].size()>DENSITY)
                  {
                      cluster_inc[t2].core=1;
                  }
                  if(similarity_matrix_inc[t2].size()<=DENSITY)
                  {
                      cluster_inc[t2].core=0;
                  }
               }

                for(int i=nrow1;i<nrow;i++)
                 {
                    if(similarity_matrix_inc[i].size() > DENSITY)
                    {
                       cluster_inc[i].core=1;                     
                    }
                    else
                    {
                       cluster_inc[i].core=0;
                    } 
                 } 

              double t2_core_inc=(double)clock()/CLOCKS_PER_SEC;

              //Clear the core/non-core vectors later             

              for(int i=0;i<cluster_inc.size();i++)
              {
                 if(cluster_inc[i].core==1)
                  core_inc.push_back(i);
                 else
                  non_core_inc.push_back(i);
              } 

             //double t2_core_inc=(double)clock()/CLOCKS_PER_SEC;
              

                          
                          cout<<"\nNew Core points:\n";
			     /*  for(int i=0;i<core_inc.size();i++)
			       {
				 cout<<core_inc[i]<<" ";

			       }*/

                          
                          cout<<"\nNew Non-Core points:\n";
			    /* for(int i=0;i<non_core_inc.size();i++)
			       {
				 cout<<non_core_inc[i]<<" ";

			       } */

              cout<<(t2_core_inc-t1_core_inc);
                          
                     

                                        //************CLUSTER FORMATION**************//


          //Update Cluster status freshly         
          double t1_cluster_inc=(double)clock()/CLOCKS_PER_SEC;  
          
          //Setting initial cluster status
           for(int i=0;i<=nrow-1;i++)
	       {
		  if(similarity_matrix_inc[i].size() > DENSITY)// core pt. check
		  {
		     cluster_inc[i].status=1;
		     cluster_inc[i].rep=i;
		     cluster_inc[i].core=1;
		  }

		  else
		  {
		     cluster_inc[i].status=0;
		     cluster_inc[i].rep=-1;
		     cluster_inc[i].core=0;

		  }
		
	       }

              /*cout<<"Initial Clusters:\n";
	       for(int i=0;i<=nrow-1;i++)
	       {
		     cout<<i<<"] ";
		     cout<<cluster_inc[i].status<<"/";
		     cout<<cluster_inc[i].rep<<"/";
		     cout<<cluster_inc[i].core<<endl;

	       }*/


		if(core_inc.size()==0)
		{
		  cout<<"\nNo additional clusters possible\n";
		
		   cout<<"\nNoise points:\n";             
		       //int noise=0;
		       double t1_noise_inc=(double)clock()/CLOCKS_PER_SEC;
		       for(int i=0;i<=nrow-1;i++)
		       {
			  if(cluster_inc[i].core==0 && cluster_inc[i].rep==-1)
			   {
			     noise_inc.push_back(i);
			     //cout<<i<<" ";    
			   }
		       }
		      //cout<<endl;                                       
		      double t2_noise_inc=(double)clock()/CLOCKS_PER_SEC;
		      cout<<(t2_noise_inc-t1_noise_inc);
			 
		       
				 
		    double total_time=(t2_data_inc-t1_data_inc)+(t2_NN_inc-t1_NN_inc)+(t2_type1-t1_type1)+(t2_sim_inc-t1_sim_inc)+(t2_core_inc-t1_core_inc)+(t1_cluster_inc-t1_cluster_inc)+(t2_noise_inc-t1_noise_inc);

		 cout<<"\nWriting values...\n";
		 double t1_write=(double)clock()/CLOCKS_PER_SEC;
		 write_binary_values();
		 double t2_write=(double)clock()/CLOCKS_PER_SEC;
                 cout<<(t2_write-t1_write);
                 write_values(input_file_string);


                 cout<<"\nTotal time: "<<t_fetch+total_time+(t2_write-t1_write)<<"/"<<(t2_NN_inc-t1_NN_inc)<<endl;

                 //Write Results
                 ofstream time_batch3("Time_batch3",ios::app);
		 time_batch3<<t_fetch+total_time+(t2_write-t1_write)<<endl;
		 time_batch3.close();

                 //Type1 Type 2 percentage
		 ofstream type1_batch3("Type1_batch3",ios::app);
		   type1_batch3<<((double)type1.size()/nrow)*100<<endl;
		   type1_batch3.close();

                 ofstream type2_batch3("Type2_batch3",ios::app);
		   type2_batch3<<((double)type2.size()/nrow)*100<<endl;
		   type2_batch3.close();

                 //Type1 Type 2 numbers
                ofstream type1_num("Type1_points",ios::app);
		   type1_num<<type1.size()<<endl;
		   type1_num.close();		 

		 ofstream type2_num("Type2_points",ios::app);
		   type2_num<<type2.size()<<endl;
		   type2_num.close();


                  //Memory Footprint
		     ofstream mem_batch3("Mem_batch3",ios::app);
		     void process_mem_usage(double&, double&);
		     double vm, rss;
		     process_mem_usage(vm, rss);
		     mem_batch3 << vm <<" "<< rss << endl;

	  
		  exit(0);
		}


          //Core pt. cluster formation
          int min;
	  for(int i=0;i<nrow;i++)
          {
             if(similarity_matrix_inc[i].size() > DENSITY)// core pt. check
	     {
                for(int j=0;j<similarity_matrix_inc[i].size();j++)
	             {
	                if(similarity_matrix_inc[similarity_matrix_inc[i][j].first].size() > DENSITY)// core pt. check
	                {
	                   temp_cluster.push_back(cluster_inc[similarity_matrix_inc[i][j].first].rep);
	                }                  
	             }

                     if(temp_cluster.size() == 0)//(single core pt. cluster)no link with other core pts.
                     {
                         cluster_inc[i].status = 1;
                         cluster_inc[i].rep = i;
                         cluster_inc[i].core = 1;
                         continue;
                     }

	             min = *min_element(temp_cluster.begin(),temp_cluster.end()); // find the min. rep. of all pts.
	             
	             if(cluster_inc[i].rep < min)
	             { 
	                cluster_inc[i].rep = cluster_inc[i].rep;
                        cluster_inc[i].status = 1;
	                for(int j=0;j<similarity_matrix_inc[i].size();j++)
	                {
	                   if(similarity_matrix_inc[similarity_matrix_inc[i][j].first].size() > DENSITY)// core pt. check
			        {
                                   link_cluster(cluster_inc[similarity_matrix_inc[i][j].first].rep,cluster_inc[i].rep);
			           cluster_inc[similarity_matrix_inc[i][j].first].rep = cluster_inc[i].rep;
                                   cluster_inc[similarity_matrix_inc[i][j].first].status = 1; 
			        }

	                }

	             }

	             if(cluster_inc[i].rep >= min)
	             { 
                        link_cluster(cluster_inc[i].rep,min);
	                cluster_inc[i].rep = min;
	                for(int j=0;j<similarity_matrix_inc[i].size();j++)
	                {
	                   if(similarity_matrix_inc[similarity_matrix_inc[i][j].first].size() > DENSITY)// core pt. check
			        {
                                   link_cluster(cluster_inc[similarity_matrix_inc[i][j].first].rep,min);
			           cluster_inc[similarity_matrix_inc[i][j].first].rep = min;
                                   cluster_inc[similarity_matrix_inc[i][j].first].status = 1;
			        }

	                }

	             }

             }//end if 

                 temp_cluster.erase(temp_cluster.begin(),temp_cluster.end());


          }//end for

          //Non-Core pt. cluster formation
          priority_queue< pair<int,int> >nearest_core;
          int nearest;
          for(int i=0;i<nrow;i++)
          {
              if(similarity_matrix_inc[i].size() <= DENSITY)// non-core pt. check
		  {		 
                     if(similarity_matrix_inc[i].size() == 0)
                       continue;
                     
		     for(int j=0;j<similarity_matrix_inc[i].size();j++)
		     {
		        if(similarity_matrix_inc[similarity_matrix_inc[i][j].first].size() > DENSITY)// core pt. check
		        nearest_core.push(pair<int,int>(similarity_matrix_inc[i][j].second,similarity_matrix_inc[i][j].first));		        
		     } 
                    
                     if(nearest_core.size() > 0){
                     nearest = nearest_core.top().second;
                     cluster_inc[i].rep = cluster_inc[nearest].rep;
                     cluster_inc[i].status = 1;
                     } 

		  
              }//end if

              while(!nearest_core.empty())
              nearest_core.pop();

          }//end for
          
          double t2_cluster_inc=(double)clock()/CLOCKS_PER_SEC;
                               
         cout<<"\nNew Clusters:\n";
	       /*for(int i=0;i<=nrow-1;i++)
	       {
		     cout<<i<<"] ";
		     cout<<cluster_inc[i].status<<"/";
		     cout<<cluster_inc[i].rep<<"/";
		     cout<<cluster_inc[i].core<<endl;

	       }*/


         cout<<(t2_cluster_inc-t1_cluster_inc);     
       
       cout<<"\nNoise points:\n";             
       //int noise=0;
       double t1_noise_inc=(double)clock()/CLOCKS_PER_SEC;
       for(int i=0;i<=nrow-1;i++)
       {
          if(cluster_inc[i].core==0 && cluster_inc[i].rep==-1)
           {
             noise_inc.push_back(i);
             //cout<<i<<" ";    
           }
       }
      //cout<<endl;                                       
      double t2_noise_inc=(double)clock()/CLOCKS_PER_SEC;
      cout<<(t2_noise_inc-t1_noise_inc);
         
       
                 
    double total_time=(t2_data_inc-t1_data_inc)+(t2_NN_inc-t1_NN_inc)+(t2_type1-t1_type1)+(t2_sim_inc-t1_sim_inc)+(t2_core_inc-t1_core_inc)+(t2_cluster_inc-t1_cluster_inc)+(t2_noise_inc-t1_noise_inc);

		 cout<<"\nWriting values...\n";
		 double t1_write=(double)clock()/CLOCKS_PER_SEC;
		 write_binary_values();
		 double t2_write=(double)clock()/CLOCKS_PER_SEC;
                 cout<<(t2_write-t1_write);
                 write_values(input_file_string);


                 cout<<"\nTotal time: "<<t_fetch+total_time+(t2_write-t1_write)<<"/"<<(t2_NN_inc-t1_NN_inc)<<endl;

        //Write Results
         ofstream time_batch3("Time_batch3",ios::app);
	 time_batch3<<t_fetch+total_time+(t2_write-t1_write)<<endl;
	 time_batch3.close();

         ofstream type1_batch3("Type1_batch3",ios::app);
	   type1_batch3<<((double)type1.size()/nrow)*100<<endl;
	   type1_batch3.close();

	 ofstream type2_batch3("Type2_batch3",ios::app);
	   type2_batch3<<((double)type2.size()/nrow)*100<<endl;
	   type2_batch3.close();

          //Type1 Type 2 numbers
         ofstream type1_num("Type1_points",ios::app);
	   type1_num<<type1.size()<<endl;
	   type1_num.close();		 

         ofstream type2_num("Type2_points",ios::app);
	   type2_num<<type2.size()<<endl;
	   type2_num.close();

        


          //Clear Type1
            type1.erase(type1.begin(),type1.end());
          //Clear Type2
            type2.erase(type2.begin(),type2.end());
          //Clear Type3   
            type3.erase(type3.begin(),type3.end());


     //Memory Footprint
     ofstream mem_batch3("Mem_batch3",ios::app);
     void process_mem_usage(double&, double&);
     double vm, rss;
     process_mem_usage(vm, rss);
     mem_batch3 << vm <<" "<< rss << endl;

    return 0;
}

//Func. to link clusters
void link_cluster(int rp,int rq)
{
   if(rp<rq)
   {
      for(int i=0;i<=nrow-1;i++)  
      {
        if(cluster_inc[i].rep==rq)
         cluster_inc[i].rep=rp;
      }
   }

   if(rq<rp)
   {
      for(int i=0;i<=nrow-1;i++)  
      {
        if(cluster_inc[i].rep==rp)
         cluster_inc[i].rep=rq;
      }
   }
}//end func



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
}//end func

//all snn reachable pts.
void BFS_snn(int s)
{
	  if(visit[s]==0)
	   visit[s]=1;

	  int v;
	  queue<int>adj_vertex;  
	  for(int j=0;j<similarity_matrix_inc[s].size();j++) //Adjacent vetices of t in the Queue
	  { 
	     v=similarity_matrix_inc[s][j].first;
	     if(visit[v]==0)//if unvisited
	     {
		  if(snn_conn(s,v))//valid snn conn.
		  {
		    adj_vertex.push(v);     
		    visit[v]=1;

                    if(v>=nrow1 && v<=nrow-1) //new pts.
                     continue;

                    if(binary_search(type1.begin(),type1.end(),v))
                     continue;

                    if(binary_search(type2.begin(),type2.end(),v))
                     continue;

                    if(!binary_search(type3.begin(),type3.end(),v)) //find type3 pts.
                     type3.push_back(v);                    
		  }

	     }//end if
	  }//end for

	  while(!adj_vertex.empty())
	  {
	    v=adj_vertex.front();
	    if(visit[v]==1)
	    {
               BFS_snn(v);
               adj_vertex.pop();
	    }
	    
	  }

   
}//end func

//Check for valid snn connection
int snn_conn(int s, int v) 
{
   int topic_s,topic_v;
   int noise_s,noise_v;

   if(binary_search(core_inc.begin(),core_inc.end(),s))
    topic_s=1;
   else
    topic_s=0;
   if(binary_search(core_inc.begin(),core_inc.end(),v))
    topic_v=1;
   else
    topic_v=0;


   if(binary_search(noise_inc.begin(),noise_inc.end(),s))
    noise_s=1;
   else
    noise_s=0;
   if(binary_search(noise_inc.begin(),noise_inc.end(),v))
    noise_v=1;
   else
    noise_v=0;

   if((topic_s==1 || topic_v==1) && (noise_s==0 && noise_v==0))    
    return 1;
   else
    return 0;

}//end func

//a snn reachable pt.
void BFS_split(int s,int d)
{
       //cout<<s<<d;
       if(visit[s]==0)
	   visit[s]=1;

	  int v;
	  queue<int>adj_vertex;  
	  for(int j=0;j<similarity_matrix_inc[s].size();j++) //Adjacent vetices of t in the Queue
	  { 
	     v=similarity_matrix_inc[s][j].first;
             //cout<<v;
	     if(visit[v]==0)//if unvisited
	     {
		  if(snn_conn(s,v))//check valid snn connection
		  {
		    adj_vertex.push(v);// add to queue     
		    visit[v]=1;
                    if(v==d){
                     snn_reach=1; //vertex found
                    }                     
		  }

	     }//end if
	  }//end for

          
	  while(!adj_vertex.empty())
	  {
	    v=adj_vertex.front();
	    if(visit[v]==1)
	    {
               if(snn_reach==1)break;
               BFS_split(v,d);
               if(snn_reach==1)break;
               adj_vertex.pop();
	    }
	    
	  }

  

}//end func

void non_core_alloc(int i)
{
  priority_queue< pair<int,int> >nearest_core;
  int nearest;

  if(similarity_matrix_inc[i].size()>0)
  {
     for(int j=0;j<similarity_matrix_inc[i].size();j++)
     {
        if(similarity_matrix_inc[similarity_matrix_inc[i][j].first].size() > DENSITY)// core pt. check
          nearest_core.push(pair<int,int>(similarity_matrix_inc[i][j].second,similarity_matrix_inc[i][j].first));		        
     } 

     if(nearest_core.size() > 0){
     nearest = nearest_core.top().second;
     cluster_inc[i].rep = cluster_inc[nearest].rep;
     cluster_inc[i].status = 1;
     } 

  }

  while(!nearest_core.empty())
  nearest_core.pop();

}//end func

void core_alloc(int i)
{
   vector<int>temp_cluster;
   int min;

    if(similarity_matrix_inc[i].size() > DENSITY)// core pt. check
	 {
	             for(int j=0;j<similarity_matrix_inc[i].size();j++)
	             {
	                if(similarity_matrix_inc[similarity_matrix_inc[i][j].first].size() > DENSITY)// core pt. check
	                {
                           snn_reach=0;
                           if(cluster_inc[similarity_matrix_inc[i][j].first].rep == similarity_matrix_inc[i][j].first)
                            snn_reach=1;
                           else{
                           snn_reach=0; for(int i1=0;i1<nrow;i1++)visit[i1]=0; 
                           BFS_split(cluster_inc[similarity_matrix_inc[i][j].first].rep,similarity_matrix_inc[i][j].first);
                           }

                           if(snn_reach==1)
	                   temp_cluster.push_back(cluster_inc[similarity_matrix_inc[i][j].first].rep);
                           else
                           {
                             cluster_inc[similarity_matrix_inc[i][j].first].status=1;
                             cluster_inc[similarity_matrix_inc[i][j].first].rep=similarity_matrix_inc[i][j].first;
                             cluster_inc[similarity_matrix_inc[i][j].first].core=1;
                             temp_cluster.push_back(cluster_inc[similarity_matrix_inc[i][j].first].rep);
                           }
	                }//end if                  
	             }

                     if(temp_cluster.size() == 0)//(single core pt. cluster)no link with other core pts.
                     {
                         cluster_inc[i].status = 1;
                         cluster_inc[i].rep = i;
                         cluster_inc[i].core = 1;
                     }

                    if(temp_cluster.size() > 0){

	             min = *min_element(temp_cluster.begin(),temp_cluster.end()); // find the min. rep. of all pts.	             
	             if(cluster_inc[i].rep < min)
	             { 
	                cluster_inc[i].rep = cluster_inc[i].rep;
                        cluster_inc[i].status = 1;
                        
	                for(int j=0;j<similarity_matrix_inc[i].size();j++)
	                {
	                   if(similarity_matrix_inc[similarity_matrix_inc[i][j].first].size() > DENSITY)// core pt. check
			        {
                                   link_cluster(cluster_inc[similarity_matrix_inc[i][j].first].rep,cluster_inc[i].rep);
			           cluster_inc[similarity_matrix_inc[i][j].first].rep = cluster_inc[i].rep;
                                   cluster_inc[similarity_matrix_inc[i][j].first].status = 1; 
			        }

	                }

	             }

	             if(cluster_inc[i].rep >= min)
	             { 
	                link_cluster(cluster_inc[i].rep,min);
                        cluster_inc[i].rep = min;
                        
	                for(int j=0;j<similarity_matrix_inc[i].size();j++)
	                {
	                   if(similarity_matrix_inc[similarity_matrix_inc[i][j].first].size() > DENSITY)// core pt. check
			        {
			           link_cluster(cluster_inc[similarity_matrix_inc[i][j].first].rep,min);
                                   cluster_inc[similarity_matrix_inc[i][j].first].rep = min;
                                   cluster_inc[similarity_matrix_inc[i][j].first].status = 1;
			        }

	                }

	             }

                 }//end if    

	 }//end if 

      temp_cluster.erase(temp_cluster.begin(),temp_cluster.end());


}//end func

//Write the binary values
void write_binary_values()
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
   int density;
   /*ifstream density_bin("DENSITY_global.bin",ios::in|ios::binary);
   density_bin.read((char*)&density,sizeof(density));
   cout<<endl<<density;
   density_bin.close();*/

   //Dump the data_matrix
   //init_data,inc_data

   ofstream out_data_binary("data_matrix_inc.bin",ios::out|ios::binary);
   if(!out_data_binary)
    {
      cout<<"Cannot open the data_matrix_inc.bin file\n"<<endl;
      exit(0);
    }
   //cout<<nrow<<ncol;
   out_data_binary.write((char*)&nrow,sizeof(nrow));
   out_data_binary.write((char*)&ncol,sizeof(ncol));
   //Write the other data values+incremental values
   long double temp;
   for(int i=0;i<=nrow-1;i++)
   {
	     for(int j=0;j<=ncol-1;j++)
	     {
                temp = data_matrix[i][j];
                out_data_binary.write((char*)&temp,sizeof(temp));
	     }

   }

   out_data_binary.close();
   
   /*int nr,nc;
   ifstream in_data_binary("data_matrix_inc.bin",ios::in|ios::binary);
   in_data_binary.read((char*)&nr,sizeof(nr));
   in_data_binary.read((char*)&nc,sizeof(nc));
   cout<<nr<<nc<<endl;
   //Read the other data values+incremental values
   for(int i=0;i<=nrow-1;i++)
   {
	     for(int j=0;j<=ncol-1;j++)
	     {
                in_data_binary.read((char*)&temp,sizeof(temp));
                cout<<temp<<" ";
	     }
       cout<<endl;
   }*/

   //Dump NN_matrix values
   ofstream nn_binary("NN_matrix_inc.bin",ios::out|ios::binary);
    if(!nn_binary)
    {
      cout<<"Cannot open the NN_matrix_inc.bin file\n"<<endl;
      exit(0);
    }

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
     ifstream nn("NN_matrix_inc.bin",ios::in|ios::binary);
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
     ofstream sim_matrix_binary("similarity_matrix_inc.bin",ios::out|ios::binary);
	    if(!sim_matrix_binary)
	    {
	      cout<<"Cannot open the similarity_matrix_inc.bin file\n"<<endl;
	      exit(0);
	    }

    pair<int,int>sim_pair;
    int row_size;

    for(int i=0;i<=nrow-1;i++)
	{
		  row_size=similarity_matrix_inc[i].size();
                  sim_matrix_binary.write((char*)&row_size,sizeof(row_size));
		  for(int j=0;j<similarity_matrix_inc[i].size();j++)
		  {
		     sim_pair.first = similarity_matrix_inc[i][j].first;
		     sim_pair.second = similarity_matrix_inc[i][j].second;

	             sim_matrix_binary.write((char*)&sim_pair,sizeof(sim_pair));
		  }
         
	} 

    sim_matrix_binary.close();

      /*pair<int,int>sim_temp;
	     ifstream sim("similarity_matrix_inc.bin",ios::in|ios::binary);
	     for(int i=0;i<=nrow-1;i++)
	     {
                sim.read((char*)&row_size,sizeof(row_size));
                cout<<"["<<row_size<<"]"; 
		for(int j=0;j<similarity_matrix_inc[i].size();j++)
		{
		   sim.read((char*)&sim_temp,sizeof(sim_temp));
		   cout<<sim_temp.first<<"/"<<sim_temp.second<<" ";
		}
	       cout<<endl;
	     }
              sim.close();*/


     //Dump core point values
           ofstream core_binary("core_inc.bin",ios::out|ios::binary);
	    if(!core_binary)
	    {
	      cout<<"Cannot open the core_inc.bin file\n"<<endl;
	      exit(0);
	    }

         int a;
            if(core_inc.size() == 0)core_binary.write((char*)&a,0);
           
            if(core_inc.size() > 0){
		    for(int i=0;i<core_inc.size();i++)
		     {
		       core_binary.write((char*)&core_inc[i],sizeof(core_inc[i]));	      
		     }
	      }
            core_binary.close();

        /* ifstream core_bin("core_inc.bin",ios::in|ios::binary);
            for(int i=0;i<core_inc.size();i++)
            {
               core_bin.read((char*)&a,sizeof(a));
               cout<<a<<" ";
            }
            core_bin.close();*/


         //Dump non-core point values
         ofstream non_core_binary("non_core_inc.bin",ios::out|ios::binary);
	    if(!core_binary)
	    {
	      cout<<"Cannot open the non_core_inc.bin file\n"<<endl;
	      exit(0);
	    } 

          if(non_core_inc.size() == 0)non_core_binary.write((char*)&a,0);
            if(non_core_inc.size() > 0){
		    for(int i=0;i<non_core_inc.size();i++)
		     {
		       non_core_binary.write((char*)&non_core_inc[i],sizeof(non_core_inc[i]));	      
		     }
               }
	     
           non_core_binary.close();  

        /*ifstream non_core_bin("non_core_inc.bin",ios::in|ios::binary);
            for(int i=0;i<non_core_inc.size();i++)
            {
               non_core_bin.read((char*)&a,sizeof(a));
               cout<<a<<" ";
            }
             non_core_bin.close();*/ 

        //Dump cluster values        
          ofstream cluster_binary("Cluster_inc.bin",ios::out|ios::binary);
	     if(!cluster_binary)
	     {
	      cout<<"Cannot open the Cluster_inc.bin file\n"<<endl;
	      exit(0);
	     }

	     for(int i=0;i<=nrow-1;i++)
		 {
		    cluster_binary.write((char*)&cluster_inc[i],sizeof(cluster_inc[i]));

		 }

	     cluster_binary.close();

          /*ifstream cluster("Cluster_inc.bin",ios::out|ios::binary);
             Cluster temp_cluster;
             for(int i=0;i<=nrow-1;i++)
             {
               cluster.read((char*)&temp_cluster,sizeof(temp_cluster));
               cout<<temp_cluster.status<<"/"<<temp_cluster.rep<<"/"<<temp_cluster.core<<endl;
             }*/

    

            //Dump Row Column values

		   ofstream rowcol_binary("Row_Column_inc.bin",ios::out|ios::binary);
		      rowcol_binary.write((char*)&nrow,sizeof(nrow));
		      rowcol_binary.write((char*)&ncol,sizeof(ncol));
		      rowcol_binary.close();

		   /*ifstream rowcol("Row_Column_inc.bin",ios::in|ios::binary);
		     rowcol.read((char*)&nrow,sizeof(nrow));
		     rowcol.read((char*)&ncol,sizeof(ncol));
		     cout<<nrow<<ncol;
		     rowcol.close();*/




 }//end func


//Write the ASCII values
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
  ofstream out_data_matrix_global("data_matrix_inc");
   if(!out_data_matrix_global)
    {
      cout<<"Cannot open the data_matrix_inc file\n"<<endl;
      exit(0);
    }
    out_data_matrix_global<<nrow<<" "<<ncol<<endl;
    int i1,j1;
    for(i1=0;i1<nrow-1;i1++)
    {
      for(j1=0;j1<ncol;j1++)
      {
         out_data_matrix_global<<data_matrix[i1][j1]<<" ";
      }
     out_data_matrix_global<<endl;
    }
    for(j1=0;j1<ncol;j1++)
    out_data_matrix_global<<data_matrix[i1][j1]<<" ";

     //Dump NN_matrix values

	    ofstream out_nn_matrix_global("NN_matrix_inc");
	    if(!out_nn_matrix_global)
	    {
	      cout<<"Cannot open the NN_matrix_inc file\n"<<endl;
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
	    ofstream out_sim_matrix_global("similarity_matrix_inc");
	    if(!out_sim_matrix_global)
	    {
	      cout<<"Cannot open the similarity_matrix_inc file\n"<<endl;
	      exit(0);
	    }

          for(int i=0;i<=nrow-1;i++)
       		{
		  
		  for(int j=0;j<similarity_matrix_inc[i].size();j++)
		  {
		     out_sim_matrix_global<<similarity_matrix_inc[i][j].first<<"/";
		     out_sim_matrix_global<<similarity_matrix_inc[i][j].second<<" ";
		  }
	         if(i<nrow-1)
		 out_sim_matrix_global<<endl;
       		} 


	    out_sim_matrix_global.close();
    
           //Dump core point values
	    ofstream out_core_global("core_inc");
	    if(!out_core_global)
	    {
	      cout<<"Cannot open the core_inc file\n"<<endl;
	      exit(0);
	    }

            if(core_inc.size() == 0)out_core_global<<"";

            if(core_inc.size() > 0){

	    for(i=0;i<core_inc.size()-1;i++)
	     {
               out_core_global<<core_inc[i]<<endl;	      
	     }
	     out_core_global<<core_inc[i];	     
	     out_core_global.close();

            }

            //Dump non-core point values
	    ofstream out_non_core_global("non_core_inc");
	    if(!out_non_core_global)
	    {
	      cout<<"Cannot open the non_core_inc file\n"<<endl;
	      exit(0);
	    }

            if(non_core_inc.size() == 0)out_non_core_global<<"";

            if(non_core_inc.size() > 0){

		    for(i=0;i<non_core_inc.size()-1;i++)
		     {
		       out_non_core_global<<non_core_inc[i]<<endl;	      
		     }
		     out_non_core_global<<non_core_inc[i];	     
		     out_non_core_global.close();

            }

          //Dump cluster values
	     ofstream out_cluster_global("Cluster_inc");
	     if(!out_cluster_global)
	     {
	      cout<<"Cannot open the Cluster_inc file\n"<<endl;
	      exit(0);
	     }
             int p;
	     for(p=0;p<nrow-1;p++)
		 {
                   out_cluster_global<<p<<"]";
		   out_cluster_global<<cluster_inc[p].status<<"/";
		   out_cluster_global<<cluster_inc[p].rep<<"/";
		   out_cluster_global<<cluster_inc[p].core;
		   out_cluster_global<<endl;

		 }
              out_cluster_global<<p<<"]";
	      out_cluster_global<<cluster_inc[p].status<<"/";
	      out_cluster_global<<cluster_inc[p].rep<<"/";
	      out_cluster_global<<cluster_inc[p].core;

             //out_cluster_global<<endl<<"Cluster status/Cluster Rep./Core-NonCore";
	      out_cluster_global.close();
      
	      ofstream out_nrow_ncol("Row_Column");
	      out_nrow_ncol<<nrow<<" "<<ncol;
	      out_nrow_ncol.close();



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






























