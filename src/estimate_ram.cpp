#include <stdio.h>
#include <stdlib.h>

#include <string>

#include "betting_abstraction.h"
#include "betting_abstraction_params.h"
#include "betting_trees.h"
#include "buckets.h"
#include "card_abstraction.h"
#include "card_abstraction_params.h"
#include "cfr_config.h"
#include "cfr_params.h"
#include "constants.h"
#include "files.h"
#include "game.h"
#include "game_params.h"
#include "io.h"
#include "params.h"
#include "tcfr.h"

using std::string;

static void Walk(Node *node, const Buckets &buckets, const CFRConfig &cfr_config,
		 long long int *num_bytes, long long int *num_flop_bytes) {
  if (node->Terminal()) return;
  int st = node->Street();
  int nb = buckets.NumBuckets(st);
  // regret bytes
  int rb;
  if (cfr_config.CharQuantizedStreet(st)) {
    rb = 1;
  } else if (cfr_config.ShortQuantizedStreet(st)) {
    rb = 2;
  } else {
    rb = 4;
  }
  int num_succs = node->NumSuccs();
  // Assumes we are maintaining sumprobs on all streets
  // sumprobs are always 4 bytes
  long long int bytes = nb * (rb + 4) * num_succs;
  *num_bytes += bytes;
  if (st == 1) *num_flop_bytes += bytes;
  // fprintf(stderr, "Adding %i\n", nb * (rb + 4) * num_succs);
  for (int s = 0; s < num_succs; ++s) {
    Walk(node->IthSucc(s), buckets, cfr_config, num_bytes, num_flop_bytes);
  }
}

static void Usage(const char *prog_name) {
  fprintf(stderr, "USAGE: %s <game params> <card params> <betting params> <cfr params>\n",
	  prog_name);
  exit(-1);
}

int main(int argc, char *argv[]) {
  if (argc != 5) Usage(argv[0]);
  Files::Init();
  unique_ptr<Params> game_params = CreateGameParams();
  game_params->ReadFromFile(argv[1]);
  Game::Initialize(*game_params);
  unique_ptr<Params> card_params = CreateCardAbstractionParams();
  card_params->ReadFromFile(argv[2]);
  unique_ptr<CardAbstraction>
    card_abstraction(new CardAbstraction(*card_params));
  unique_ptr<Params> betting_params = CreateBettingAbstractionParams();
  betting_params->ReadFromFile(argv[3]);
  unique_ptr<BettingAbstraction>
    betting_abstraction(new BettingAbstraction(*betting_params));
  unique_ptr<Params> cfr_params = CreateCFRParams();
  cfr_params->ReadFromFile(argv[4]);
  unique_ptr<CFRConfig> cfr_config(new CFRConfig(*cfr_params));
  Buckets buckets(*card_abstraction, true);
  BettingTrees betting_trees(*betting_abstraction);
  long long int num_bytes = 0LL, num_flop_bytes = 0LL;
  Walk(betting_trees.Root(), buckets, *cfr_config, &num_bytes, &num_flop_bytes);
  printf("%lli bytes\n", num_bytes);
  printf("%lli flop bytes\n", num_flop_bytes);
}
