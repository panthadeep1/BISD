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
vector<int>data_del;
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
int nrow1,nrow2,nrow,w=2;
int ncol,ncol2,snn_reach=0;
double total_time=0,Time=0;
double type12=0;


typedef struct Clust{

     public: 
      int status;
      int rep;
      int core;
}Cluster;
vector<Cluster>cluster;
vector<Cluster>cluster_inc;
vector<int>temp_cluster;

bool myfunction (std::pair<long double,int> a,std::pair<long double,int> b) { return (a.second<b.second); }

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
    
 
    //Read the rest of the values from the input binary file
 long double temp;
          int row_size_data;
	  for(int i=0;i<=nrow1-1;i++)
	    {
               in.read((char*)&row_size_data,sizeof(row_size_data));
               if(row_size_data == 0) continue;
              
               if(row_size_data > 0)
               { 
		       for(int j=0;j<=ncol-1;j++)
		       {
			 in.read((char*)&temp,sizeof(long double));
			 data_matrix[i].push_back(temp);
		       }
               }

	    }
     double t2_data_fetch=(double)clock()/CLOCKS_PER_SEC;
	/* cout<<"The Original data matrix is:\n";
	   for(int i=0;i<=nrow1-1;i++)
	   {
	      for(int j=0;j<=ncol-1;j++)
	      {
		 cout<<data_matrix[i][j]<<" ";
	      }
	    
	     cout<<endl;
	   }*/
	 
	   in.close();

      cout<<(t2_data_fetch-t1_data_fetch);    


 

                                             //********Fetching the Decremental data file*******//   


  double t1_inc_data=(double)clock()/CLOCKS_PER_SEC;
  string inc_file = argv[2];
  string inc_file_string = inc_file;
  string inc_file_bin = inc_file+".bin";
  inc_data = inc_file_bin; 

  ifstream inc(inc_file_bin.c_str()); 
  if(!inc)
  {
    cout<<"Cannot open the decremental file: "<<inc_file_bin<<endl;
    exit(0);
  }

  //Read the nrow,ncol values from input decremental binary file
  
    inc.read((char*)&nrow2,sizeof(int));
    inc.read((char*)&ncol2,sizeof(int));
 
  cout<<"\nDecremental file:"<<inc_file_bin<<"\n";
  cout<<"Rows:"<<nrow2;
  cout<<" Columns:"<<ncol2<<endl;
  //Check on the no. of pts. to be deleted
  if(nrow1-nrow2<KNN)
  {
    cout<<"Delete <= "<<(2-1)*KNN<<" points or appropriate, TRY AGAIN!!!."; //Fix the no. of pts. to be deleted
    exit(0);
  } 
   
                                             
  //Read the rest of the values from the non-incremental binary file
  long double temp1;
  for(int i=0;i<=nrow2-1;i++)
    {
      inc.read((char*)&temp1,sizeof(temp1)); //unsigned
      data_del.push_back((int)temp1);
    }

   inc.close();

   double t2_inc_data=(double)clock()/CLOCKS_PER_SEC;
   cout<<"The decremental data fetch is: "<<data_del.size()<<" points. "<<endl;
  
   
   cout<<(t2_inc_data-t1_inc_data);     


                                                //*******Fetching the original NN_matrix********// 


   double t1_NN_fetch=(double)clock()/CLOCKS_PER_SEC;
   NN_matrix.resize(nrow1);
    //for(int i=0;i<=nrow1-1;i++)
      //NN_matrix[i].resize(2*KNN);

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
   int row_size_nn;
   for(int i=0;i<=nrow1-1;i++)
   {
         nn_matrix.read((char*)&row_size_nn,sizeof(row_size_nn));
             if(row_size_nn == 0) continue;
    
         if(row_size_nn > 0)
         {
	     for(int j=0;j<row_size_nn;j++)
	     {
		nn_matrix.read((char*)&nn_temp,sizeof(nn_temp));
		NN_matrix[i].push_back(nn_temp);
	     }
      
         }

   }
   nn_matrix.close();
   double t2_NN_fetch=(double)clock()/CLOCKS_PER_SEC;
   cout<<"\nThe Original NN matrix is:\n";
   /*for(int i=0;i<=nrow1-1;i++)
          {
		cout<<i<<"] ";    
		    for(int j=0;j<=2*KNN-1;j++)
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
         int row_size_sim;
                 for(int i=0;i<=nrow1-1;i++)
                 {
                    sim_matrix.read((char*)&row_size_sim,sizeof(row_size_sim));
                    if(row_size_sim == 0) continue;

                    if(row_size_sim > 0)
                    {
                      for(int j=0;j<row_size_sim;j++)
                       {
                          sim_matrix.read((char*)&sim_temp,sizeof(sim_temp));
                          similarity_matrix[i].push_back(sim_temp);

                       }
                    }

                 }
         double t2_sim_fetch=(double)clock()/CLOCKS_PER_SEC;
         cout<<"\nThe Original Similarity Matrix is:\n";
		/*   for(int i=0;i<=nrow1-1;i++)
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
         cout<<(t2_sim_fetch-t1_sim_fetch);         
         //Working Similarity matrix
         if(init_data=="data_matrix_global.bin")
         sim_matrix.open("similarity_matrix_global.bin",ios::in|ios::binary);
         else
         sim_matrix.open("similarity_matrix_inc.bin",ios::in|ios::binary);
         if(!sim_matrix)
         {
            cout<<"Cannot open the similarity_matrix_global.bin"<<endl;
            //cout<<init_data; 
            exit(0);

         }
            for(int i=0;i<=nrow1-1;i++)
                 {
                    sim_matrix.read((char*)&row_size_sim,sizeof(row_size_sim));
                    if(row_size_sim == 0) continue;

                    if(row_size_sim > 0)
                    {
                      for(int j=0;j<row_size_sim;j++)
                       {
                          sim_matrix.read((char*)&sim_temp,sizeof(sim_temp));
                          similarity_matrix_inc[i].push_back(sim_temp);

                       }
                    }

                 }

            sim_matrix.close();
     

             
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
                  
               }
               non_core_global.close();
               double t2_non_core_fetch=(double)clock()/CLOCKS_PER_SEC;  
               cout<<"\nOriginal Non-Core points:\n";
	       /*for(int i=0;i<non_core.size();i++)
	       {
		 cout<<non_core[i]<<" ";

	       }*/  

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
                   cluster_inc[i]=cluster[i]; //Assigning to new working cluster vector
                   
                }
                double t2_cluster_fetch=(double)clock()/CLOCKS_PER_SEC;
                 cout<<"\nOriginal Clusters:\n";
		      /* for(int i=0;i<=nrow1-1;i++)
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


                cout<<"\nTotal fetch time:"<<t_fetch<<endl; 


    
                                             //<----------DECREMENTAL PHASE---------->//


    cout<<endl<<"\nDecremental Phase begins...\n";

                                        

   int dp;
   int del_pt;
   
     
                                       //*************Update the data matrix***************//


           double t1_data_inc=(double)clock()/CLOCKS_PER_SEC;

           //Erase the deleted pt.  
           for(dp=0;dp<data_del.size();dp++)
           { 
              del_pt = data_del[dp]; 
              data_matrix[del_pt].erase(data_matrix[del_pt].begin(),data_matrix[del_pt].end());
           }
           //cout<<data_matrix[del_pt].size();

          double t2_data_inc=(double)clock()/CLOCKS_PER_SEC;
          cout<<"Data matrix updated in: "<<(t2_data_inc-t1_data_inc)<<endl;



                                    //**************Update the NN matrix****************//
           
           double t1_NN_inc=(double)clock()/CLOCKS_PER_SEC; 

           //Erase the e-KNN list of deleted pt.
            for(dp=0;dp<data_del.size();dp++)
            { 
               del_pt = data_del[dp];
               NN_matrix[del_pt].erase(NN_matrix[del_pt].begin(),NN_matrix[del_pt].end());
            }
           //cout<<NN_matrix[del_pt].size(); 


	  //Identifying the Type1 affected pts. and updating the KNN list of type1 pts.
           for(int i=0;i<nrow1;i++)
		       {

				 if(data_matrix[i].size() > 0) //'i' is an existing pt.
				 {
				   int j=0;
				    for(j;j<KNN && j<NN_matrix[i].size();)
				    { 
					if(NN_matrix[i].size() == 0)
					  break;

					int k=NN_matrix[i][j].second;  
					if(data_matrix[k].size() == 0) // 'k' is a current or previously deleted pt.
					{
					   if(!binary_search(type1.begin(),type1.end(),i))// find type-1 pts.
					     type1.push_back(i);

					   NN_matrix[i].erase(NN_matrix[i].begin()+j); //remove 'k' from the KNN list of 'i'
					   //j=0;
					   continue;
					}
					else 
					  j++;
				
				    }// end for

				 }//end if


		       }//end for

           //Labelling Type1 points.
            cout<<"Type 1 points: "<<type1.size()/(double)(nrow1-data_del.size())*100<<"%"<<endl;

            //Extending the shrinked KNN list to (w * KNN) for type1 pts. for next iteration         
               int max_index;
               long double max_dist;
               int l=0;
	       long double dist_ij,sum=0,sum1=0,sum2=0;
	       pair<long double,int> d_ij;
              
                for(int t=0;t<type1.size();t++)
                {
                      int t1=type1[t];
                       if(data_matrix[t1].size() > 0) // 't1' is an existing pt.
                        {
                          
                            if(NN_matrix[t1].size() < w*KNN) //fill the shrinked KNN list of 't1' to w*KNN
                             {
		                 for(int j=0;j<nrow1;j++)// Calculate t1's distances with all other existing pts.
		                   {
		                         sum=0;

		                         if(data_matrix[j].size() == 0) // 'j' is a deleted pt.
		                          continue; 

		                         if(j == t1)
		                          continue;

		                         for(int k=0;k<=ncol-1;k++)
					     sum += pow((data_matrix[t1][k]-data_matrix[j][k]),2); //Euclidean Distance
					     dist_ij=sqrt(sum);   //sqrt  
		                 
		                         
		                 
			                 if(NN_matrix[t1].size() < w*KNN)
			                 { 
			                    NN_matrix[t1].push_back(pair<long double,int>(dist_ij,j));
			                    sort(NN_matrix[t1].begin(),NN_matrix[t1].end());
			                 }
			                 else
			                 {
		                                   max_index = NN_matrix[t1].size()-1;
		                                   if(max_index >= 0) //non-empty KNN list of 't1'
		                                     max_dist = NN_matrix[t1][max_index].first;
	    
		                                   if(dist_ij < max_dist){
		                                     NN_matrix[t1].push_back(pair<long double,int>(dist_ij,j));			                   
		                                     sort(NN_matrix[t1].begin(),NN_matrix[t1].end());
		                                     NN_matrix[t1].erase(NN_matrix[t1].begin()+(max_index+1));
		                                   }
			                    
			                 }
		                         

		                        
		                   }//end for


                           }//end if //e-KNN list created for the type 1 pt.                       
                       

                        
			      if(NN_matrix[t1].size() < KNN)
			      {
			         cout<<"\n Point "<<t1<<" does not have sufficient values in its KNN list. TRY AGAIN!!!!\n";
			         exit(0);               
			      }


		              //Finding type 2 pts.
		              for(int n=0;n<KNN;n++)
		              {
		                 int k = NN_matrix[t1][n].second;
		                 if(data_matrix[k].size() > 0)//k is an existing pt.
		                 {
		                     if(!binary_search(type1.begin(),type1.end(),k))// k is non-type 1 pt.
		                      {
		                         if(!binary_search(type2.begin(),type2.end(),k))
		                          type2.push_back(k);
		                      }
		                 }
		         
		              }//end for


                         
                     }// end if       

              

             }//end for            

              //Labelling Type2 points.
		   cout<<"Type 2 affected points:"<<type2.size()/(double)(nrow1-data_del.size())*100<<"%"<<endl;;         

            double t2_NN_inc=(double)clock()/CLOCKS_PER_SEC;
            cout<<"NN matrix updated in: "<<(t2_NN_inc-t1_NN_inc)<<endl;


        
                                                      //****Updating the Similarity Matrix*****//

                         
		 double t1_sim_inc=(double)clock()/CLOCKS_PER_SEC;
                 similarity_matrix_inc.resize(nrow1);
                 int common;
                 //Removing the similarity rows of current deleted pts.
                  for(dp=0;dp<data_del.size();dp++)
		  { 
		     del_pt = data_del[dp];
                     similarity_matrix_inc[del_pt].erase(similarity_matrix_inc[del_pt].begin(),similarity_matrix_inc[del_pt].end());
                  }

                        //Updating the similarity rows of the type-1 pts.
                         for(int i=0;i<type1.size();i++)
			   {
			       int t1=type1[i];
			       similarity_matrix_inc[t1].erase(similarity_matrix_inc[t1].begin(),similarity_matrix_inc[t1].end());
			       //cout<<t1;
			       for(int j=0;j<KNN;j++)       
			       {
			          int k = NN_matrix[t1][j].second;  
			          for(int t=0;t<KNN;t++)
			          {
		                            if(NN_matrix[k][t].second == t1){
					     common = link_strength(t1,k);
						     if(common >= SNN)
						     {
							 similarity_matrix_inc[t1].push_back(pair<int,int>(k,common));
						     }
                                            break;
                                       }
			          }
			       }//end for
	   
			   }//end for 



                       //Updating the similarity rows of the type-2 pts.
                        for(int i=0;i<type2.size();i++)
			   {
			       int t2=type2[i];
			       similarity_matrix_inc[t2].erase(similarity_matrix_inc[t2].begin(),similarity_matrix_inc[t2].end());
			       //cout<<t1;
			       for(int j=0;j<KNN;j++)       
			       {
			          int k = NN_matrix[t2][j].second;  
			          for(int t=0;t<KNN;t++)
			          {
		                            if(NN_matrix[k][t].second == t2){
					     common = link_strength(t2,k);
						     if(common >= SNN)
						     {
							 similarity_matrix_inc[t2].push_back(pair<int,int>(k,common));
						     }
                                            break;
                                       }
			          }
			       }//end for
	   
			   }//end for 
                       

                       
                       double t2_sim_inc=(double)clock()/CLOCKS_PER_SEC;
                       cout<<"Similarity matrix updated in: "<<(t2_sim_inc-t1_sim_inc)<<endl;



                                                    //*********Update Core/Non-Core pts.********//

                    core_inc.erase(core_inc.begin(),core_inc.end());
		    non_core_inc.erase(non_core_inc.begin(),non_core_inc.end());
                              
                    double t1_core_inc=(double)clock()/CLOCKS_PER_SEC; 
                     for(int i=0;i<type1.size();i++)
			    {
			       int t1=type1[i];
			       if(data_matrix[t1].size() > 0)
			       {
				if(similarity_matrix_inc[t1].size() > DENSITY) //core pt. check 
				 cluster_inc[t1].core=1;

				if(similarity_matrix_inc[t1].size() <= DENSITY) //non-core pt. check 
				 cluster_inc[t1].core=0;
			       }
			    }


                      for(int i=0;i<type2.size();i++)
			    {
			       int t2=type2[i];
			       if(data_matrix[t2].size() > 0)
			       {
				if(similarity_matrix_inc[t2].size() > DENSITY) //core pt. check 
				 cluster_inc[t2].core=1;

				if(similarity_matrix_inc[t2].size() <= DENSITY) //non-core pt. check 
				 cluster_inc[t2].core=0;
			       }
			    }
    

                    double t2_core_inc=(double)clock()/CLOCKS_PER_SEC; 
                    cout<<"Core/Non-core lists updated in: "<<(t2_core_inc-t1_core_inc)<<endl;

                     //Create the new core, non-core list
			    for(int i=0;i<cluster_inc.size();i++)
			      {

					if(data_matrix[i].size() > 0)
					{ 
						 if(cluster_inc[i].core==1)
						  core_inc.push_back(i);
						 else
						  non_core_inc.push_back(i);
					}

			      } 


          
           total_time += (t2_data_inc-t1_data_inc)+(t2_NN_inc-t1_NN_inc)+(t2_sim_inc-t1_sim_inc)+(t2_core_inc-t1_core_inc);
  


                                                                 //RESET VALUES


                           
                       


 

                                                  //************CLUSTER FORMATION**************//


              cout<<"\nTotal time b4 Clusters: "<<t_fetch+total_time<<endl;
              
              double t1_cluster_inc=(double)clock()/CLOCKS_PER_SEC;
              cluster_inc.resize(nrow1);

              //Initialize Clusters
              for(int i=0;i<=nrow1-1;i++)
	       {
		           if(data_matrix[i].size() > 0)// existing pt.
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
		           else  //already deleted pt.
		           {
		               cluster_inc[i].status=-1;
			       cluster_inc[i].rep=-1;
			       cluster_inc[i].core=-1;                      
		           }	
	
	       } 

               if(core_inc.size()==0)
		{
                    cout<<"\nNo additional clusters possible\n";
                     double t1_noise_inc=(double)clock()/CLOCKS_PER_SEC;
		       for(int i=0;i<=nrow1-1;i++)
		       {
			  if(cluster_inc[i].core==0 && cluster_inc[i].rep==-1)
			   {
			     noise_inc.push_back(i);
			     //cout<<i<<" ";    
			   }
		       }                                      
		     double t2_noise_inc=(double)clock()/CLOCKS_PER_SEC;

                      total_time += (t2_noise_inc-t1_noise_inc);

		      cout<<"\nWriting values...\n";
		      double t1_write=(double)clock()/CLOCKS_PER_SEC;
		      write_binary_values();
		      double t2_write=(double)clock()/CLOCKS_PER_SEC;
		      cout<<(t2_write-t1_write);      
		      
		      Time = t_fetch+total_time+(t2_write-t1_write);
		      cout<<"\nTotal time: "<<Time<<endl;
		      write_values(input_file_string); 


	      		 //TOTAL TIME taken
		             ofstream time_batch3("Time_batch3",ios::app);
			     time_batch3<<Time<<endl;
			     time_batch3.close();
		             cout<<"\nTotal time: "<<Time<<endl;

			 //Type-1 points % affected
		           ofstream Type1_batch3("Type1_batch3",ios::app);
			   Type1_batch3<<type1.size()/(double)(nrow1-data_del.size())*100<<endl;
			   Type1_batch3.close();
		           cout<<"\nType1 points affected(%): "<<type1.size()/(double)(nrow1-data_del.size())*100<<endl;

                        //Type-2 points % affected
		           ofstream Type2_batch3("Type2_batch3",ios::app);
			   Type2_batch3<<type2.size()/(double)(nrow1-data_del.size())*100<<endl;
			   Type2_batch3.close();
		           cout<<"\nType2 points affected(%): "<<type2.size()/(double)(nrow1-data_del.size())*100<<endl;


			 //Memory Footprint
			     ofstream mem_batch3("Mem_batch3",ios::app);
			     void process_mem_usage(double&, double&);
			     double vm, rss;
			     process_mem_usage(vm, rss);
			     mem_batch3 << vm <<" "<< rss << endl;
	       
		            cout<<"\nTotal RSS mem used: "<<rss<<endl;
					  
			    exit(0);

        
                }//end if



                //Core pt. cluster formation
                int min;
                for(int i=0;i<=nrow1-1;i++)
		   {
		      if(data_matrix[i].size() == 0) continue;

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
             for(int i=0;i<=nrow1-1;i++)
	     {
		  if(data_matrix[i].size() == 0) continue;

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
             cout<<"Clusters formed in: "<<(t2_cluster_inc-t1_cluster_inc)<<endl; 
           
             //Noise points:
	       double t1_noise_inc=(double)clock()/CLOCKS_PER_SEC;
	       for(int i=0;i<=nrow1-1;i++)
	       {
		  if(cluster_inc[i].core==0 && cluster_inc[i].rep==-1)
		   {
		     noise_inc.push_back(i);
		     //cout<<i<<" ";    
		   }
	       }                                      
	      double t2_noise_inc=(double)clock()/CLOCKS_PER_SEC;
	      cout<<"Noise points in: "<<(t2_noise_inc-t1_noise_inc)<<endl;

              total_time += (t2_cluster_inc-t1_cluster_inc)+(t2_noise_inc-t1_noise_inc);

              cout<<"\nWriting values...\n";
	      double t1_write=(double)clock()/CLOCKS_PER_SEC;
	      write_binary_values();
	      double t2_write=(double)clock()/CLOCKS_PER_SEC;
	      cout<<(t2_write-t1_write)<<endl;  

              Time = t_fetch+total_time+(t2_write-t1_write);
	      write_values(input_file_string);  



                         //TOTAL TIME taken
		             ofstream time_batch3("Time_batch3",ios::app);
			     time_batch3<<Time<<endl;
			     time_batch3.close();
		             cout<<"\nTotal time: "<<Time<<endl;

			 //Type-1 points % affected
		           ofstream Type1_batch3("Type1_batch3",ios::app);
			   Type1_batch3<<type1.size()/(double)(nrow1-data_del.size())*100<<endl;
			   Type1_batch3.close();
		           cout<<"\nType1 points affected(%): "<<type1.size()/(double)(nrow1-data_del.size())*100<<endl;

                        //Type-2 points % affected
		           ofstream Type2_batch3("Type2_batch3",ios::app);
			   Type2_batch3<<type2.size()/(double)(nrow1-data_del.size())*100<<endl;
			   Type2_batch3.close();
		           cout<<"\nType2 points affected(%): "<<type2.size()/(double)(nrow1-data_del.size())*100<<endl;


			 //Memory Footprint
			     ofstream mem_batch3("Mem_batch3",ios::app);
			     void process_mem_usage(double&, double&);
			     double vm, rss;
			     process_mem_usage(vm, rss);
			     mem_batch3 << vm <<" "<< rss << endl;
	       
		            cout<<"\nTotal RSS mem used: "<<rss<<endl;
      
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
  n1=KNN;
  n2=KNN;

  int i=0,j=0,common=0;
	   while (i < n1 && j < n2) 
           {
             if(NN_matrix[a][i].second < NN_matrix[b][j].second)
             {		   
                i++;
                continue;
             }

             if(NN_matrix[b][j].second < NN_matrix[a][i].second)
             {		   
                j++;
                continue;
             }

             if(NN_matrix[a][i].second == NN_matrix[b][j].second)
             {		   
                common++;
                i++;
                j++;
                continue;
             }


	   }  

  return common;
}

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
    out_data_matrix_global<<nrow1<<" "<<ncol<<endl;
    int i1,j1;
    for(i1=0;i1<nrow1;i1++)
    {
      
	      for(j1=0;j1<data_matrix[i1].size();j1++)
	      {
		 out_data_matrix_global<<data_matrix[i1][j1]<<" ";
	      }
     
     if(i1<nrow1-1)
     out_data_matrix_global<<endl;
    }
    out_data_matrix_global.close();

     //Dump NN_matrix values

	    ofstream out_nn_matrix_global("NN_matrix_inc");
	    if(!out_nn_matrix_global)
	    {
	      cout<<"Cannot open the NN_matrix_inc file\n"<<endl;
	      exit(0);
	    }

        int i,j;
         for(i=0;i<nrow1;i++)
	    {
                   for(j=0;j<NN_matrix[i].size();j++)
		      {
		       out_nn_matrix_global<<NN_matrix[i][j].first<<"/";
		       out_nn_matrix_global<<NN_matrix[i][j].second<<" ";

		      }
		      
		     if(i<nrow1-1)
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

          for(int i=0;i<nrow1;i++)
       		{
		  
		  for(int j=0;j<similarity_matrix_inc[i].size();j++)
		  {
		     out_sim_matrix_global<<similarity_matrix_inc[i][j].first<<"/";
		     out_sim_matrix_global<<similarity_matrix_inc[i][j].second<<" ";
		  }
	         if(i<nrow1-1)
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
	     for(p=0;p<nrow1-1;p++)
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
   //cout<<nrow1<<ncol;
   out_data_binary.write((char*)&nrow1,sizeof(nrow1));
   out_data_binary.write((char*)&ncol,sizeof(ncol));
  //Write the other data values+incremental values
  long double temp;
   int row_size_data;
   for(int i=0;i<=nrow1-1;i++)
   {
     row_size_data=data_matrix[i].size();
     out_data_binary.write((char*)&row_size_data,sizeof(row_size_data));
     
	     for(int j=0;j<data_matrix[i].size();j++)
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
   for(int i=0;i<=nrow1-1;i++)
   {
       in_data_binary.read((char*)&row_size_data,sizeof(row_size_data));
	     for(int j=0;j<=row_size_data-1;j++)
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
    int row_size_nn; 
     for(int i=0;i<=nrow1-1;i++)
     {
       row_size_nn = NN_matrix[i].size();
       nn_binary.write((char*)&row_size_nn,sizeof(row_size_nn));
          
		for(int j=0;j<NN_matrix[i].size();j++)
		{
		   nn_pair.first = NN_matrix[i][j].first;         
		   nn_pair.second = NN_matrix[i][j].second;

		   nn_binary.write((char*)&nn_pair,sizeof(nn_pair));
		}
         
          

     }

     nn_binary.close();
     
     /*pair<long double,int>nn_temp;
     ifstream nn("NN_matrix_inc.bin",ios::in|ios::binary);
     for(int i=0;i<=nrow1-1;i++)
     {
        nn.read((char*)&row_size_nn,sizeof(row_size_nn));
        for(int j=0;j<=row_size_nn-1;j++)
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
    int row_size_sim;

    for(int i=0;i<=nrow1-1;i++)
	{
		  row_size_sim=similarity_matrix_inc[i].size();
                  sim_matrix_binary.write((char*)&row_size_sim,sizeof(row_size_sim));
		  for(int j=0;j<similarity_matrix_inc[i].size();j++)
		  {
		     sim_pair.first = similarity_matrix_inc[i][j].first;
		     sim_pair.second = similarity_matrix_inc[i][j].second;

	             sim_matrix_binary.write((char*)&sim_pair,sizeof(sim_pair));
		  }
         
	} 

    sim_matrix_binary.close();

    /* pair<int,int>sim_temp;
	     ifstream sim("similarity_matrix_inc.bin",ios::in|ios::binary);
	     for(int i=0;i<=nrow1-1;i++)
	     {
                sim.read((char*)&row_size_sim,sizeof(row_size_sim));
                cout<<"["<<row_size_sim<<"]"; 
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

	     for(int i=0;i<=nrow1-1;i++)
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
		      rowcol_binary.write((char*)&nrow1,sizeof(nrow1));
		      rowcol_binary.write((char*)&ncol,sizeof(ncol));
		      rowcol_binary.close();

		   /*ifstream rowcol("Row_Column_inc.bin",ios::in|ios::binary);
		     rowcol.read((char*)&nrow,sizeof(nrow));
		     rowcol.read((char*)&ncol,sizeof(ncol));
		     cout<<nrow<<ncol;
		     rowcol.close();*/



}//end func


//MEMORY footprint

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


















