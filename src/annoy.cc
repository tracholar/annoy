// Copyright (c) 2013 Spotify AB
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy of
// the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations under
// the License.

// Author zuoyuan@mail.ustc.edu.cn 
// annoy index for hive transform 

#include<stddef.h>
#include "kissrandom.h"
#include "annoylib.h"
#include<string>
#define MODE_INDEX 1 
#define MODE_SEARCH 2

int ArgPos(char *str, int argc, char **argv) {
  int a;
  for (a = 1; a < argc; a++) if (!strcmp(str, argv[a])) {
    if (a == argc - 1) {
      printf("Argument missing for %s\n", str);
      exit(1);
    }
    return a;
  }
  return -1;
}

int main(int argc, char **argv){
  int i;
  if (argc == 1) {
    printf("ANN search toolkit v 0.1c\n\n");
    printf("Options:\n");
    printf("\t-size <file>\n");
    printf("\t\tvector size\n");
    printf("\t-nnsize <file>\n");
    printf("\t\tANN size\n");
    printf("\t-tree <file>\n");
    printf("\t\ttree number for annoy. default 10\n");
    printf("\t-search_k <file>\n");
    printf("\t\tsearch_k for annoy. default -1\n");
    printf("\t-input <file>\n");
    printf("\t\tinput <file> default from stdin\n");
    printf("\t-output <file>\n");
    printf("\t\toutput <file> default to stdout\n");
    return 0;
  }

  int size = 0;
  int nnsize = 100;
  int tree = 10;
  int mode = MODE_INDEX;
  char input[256] = {0};
  char output[256] = {0};
  int logtimes = 10000;
  size_t search_k = (size_t)-1;
  if ((i = ArgPos((char *)"-size", argc, argv)) > 0) size = atoi(argv[i + 1]);
  if ((i = ArgPos((char *)"-nnsize", argc, argv)) > 0) nnsize = atoi(argv[i + 1]);
  if ((i = ArgPos((char *)"-tree", argc, argv)) > 0) tree = atoi(argv[i + 1]);
  if ((i = ArgPos((char *)"-mode", argc, argv)) > 0) mode = atoi(argv[i + 1]);
  if ((i = ArgPos((char *)"-logtimes", argc, argv)) > 0) logtimes = atoi(argv[i + 1]);
  if ((i = ArgPos((char *)"-search_k", argc, argv)) > 0) search_k = atoi(argv[i + 1]);
  if ((i = ArgPos((char *)"-input", argc, argv)) > 0) strncpy(input, argv[i+1], 256);
  if ((i = ArgPos((char *)"-output", argc, argv)) > 0) strncpy(output, argv[i+1], 256);

  FILE * fin = stdin;
  FILE * fout = stdout;
  if(input[0] != 0){
      fin = fopen(input, "r");
      if(fin == NULL){
          fprintf(stderr, "ERROR: open file %s failed!\n", input);
          exit(1);
      }
  }
  if(output[0] != 0){
      fout = fopen(output, "w");
      if(fout == NULL){
          fprintf(stderr, "ERROR: open file %s failed!\n", output);
          exit(1);
      }
  }
  

  AnnoyIndex<long long, float, Angular, Kiss32Random> t = AnnoyIndex<long long, float, Angular, Kiss32Random>(size);

  // read data 
  float * buff = new float[size];
  long long n = 0;
  char key[1024];
  std::vector<std::string> keys;
  while(!feof(fin)){
    int sn = fscanf(fin, "%s", key);
    if(sn <= 0) break;
    keys.push_back(std::string(key));
    for(int i=0; i<size; i++){
        fscanf(fin, "%f", &buff[i]);
    }
    t.add_item(n, buff);
    n++;
    if(n % logtimes == 0){
        fprintf(stderr, "index %lld items.\n", n);
    }
  }
  t.build(tree);
  
  std::vector<long long> closest;
  std::vector<float> distances;
  
  for(long long i=0; i<n; i++){
      t.get_nns_by_item(i, nnsize, search_k, &closest, &distances);
      fprintf(fout, "%s\t", keys[i].c_str());
      for(int j=0; j<closest.size(); j++){
          if(j > 0) fputc(' ', fout);
          fprintf(fout, "%s", keys[ closest[j] ].c_str());
      }
        
      fputc( '\t', fout);
      for(int j=0; j<distances.size(); j++){
          if(j > 0) fputc(' ', fout);
          fprintf(fout, "%g", distances[j]);
      }
      fputc('\n', fout);

      closest.clear(); 
      distances.clear();

      if((i+1) % logtimes == 0){
        fprintf(stderr, "search %lld items.\n", i);
    }
  }

  delete[] buff;
}