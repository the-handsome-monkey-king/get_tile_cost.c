/*
 * get_tile_cost.c
 *
 * Get the cost to tile a room given the tile cost,
 * width, and length of the room, as well as whether
 * partial tiles can be used to fill gaps.
 *
 * Libraries to link: math
 *
 * author: Ryan Morehouse
 * license: MIT
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void get_user_input(
    double *area_w, double *area_h, 
    double *tile_w, double *tile_h,
    int *extra_tiles, int *tile_cost,
    int *subtiles_bool)
{
  // initialize dimensions
  printf("Please enter floor width: ");
  scanf("%lf", area_w);
  printf("Please enter the floor height: ");
  scanf("%lf", area_h);
  printf("Please enter the tile width: ");
  scanf("%lf", tile_w);
  printf("Please enter the tile height: ");
  scanf("%lf", tile_h);

  // initialize tile cost
  printf("Please enter the cost per tile in dollars: ");
  double raw_tile_cost;
  scanf("%lf", &raw_tile_cost);
  *tile_cost = 100 * raw_tile_cost;

  // get any extra tiles beyond what is needed
  printf("Would you like extra tiles beyond what is needed?\n");
  printf("Please enter a number of extra tiles (or 0 for none): ");
  scanf("%d", extra_tiles);

  // initialize subtiles bool
  char use_subtiles = 'x';
  printf("Can subtiles be used (y/n): ");
  do {
    scanf("%c", &use_subtiles);
  } while (
    use_subtiles != 'y' && use_subtiles != 'n' &&
    use_subtiles != 'Y' && use_subtiles != 'N');

  if(use_subtiles == 'y' || use_subtiles == 'Y')
    *subtiles_bool = 1;
  else if(use_subtiles == 'n' || use_subtiles == 'N')
    *subtiles_bool = 0;
} 

// check user inputs
// return 1 if inputs are valid
// return 0 if inputs are invalid
int verify_input(
    double *area_w, double *area_h, 
    double *tile_w, double *tile_h,
    int *extra_tiles, int *tile_cost)
{
  if(
    *area_w <= 0 || *area_h <= 0 ||
    *tile_w <= 0 || *tile_h <= 0 ||
    *extra_tiles < 0 || *tile_cost <= 0)
  {
    printf("Values:\n");
    printf("area_w: %f\narea_h: %f\ntile_w: %f\ntile_h: %f\n"
      "extra tiles: %d\ntile_cost (in cents): %d\n", 
      *area_w, *area_h, *tile_w, *tile_h, 
      *extra_tiles, *tile_cost);
    return 0;
  } else {
    return 1;
  }
}

long get_tiles_for_one_remaining_dimension(
    double *area_d, double *tile_d, 
    double *other_area_d, double *other_tile_d,
    int *use_subtiles)
{
  double other_tile_count = ceil(
    *other_area_d / *other_tile_d);
  double remaining_tile_count = floor(
    *area_d / *tile_d);

  if(*use_subtiles == 1)
  {
    remaining_tile_count += 1;
    return (long)(other_tile_count * remaining_tile_count);
  }

  // else
  double tiles = other_tile_count * remaining_tile_count;
  
  double remaining_d = *area_d - (remaining_tile_count * *tile_d);
  double subtiles = floor(*tile_d / remaining_d);
  double additional_tiles = floor(other_tile_count / subtiles) + 1;
  tiles += additional_tiles;
  return (long)(tiles);
}

double get_tiles_for_last_dimension(
    double *area_d, double *tile_d, 
    double *other_area_d, double *other_tile_d,
    int *use_subtiles)
{
  if(*use_subtiles == 1)
    return (long)(*other_area_d / *other_tile_d);
  // else
  double remaining_d = *area_d;
  remaining_d -= *tile_d * floor(*area_d / *tile_d);
  double subtiles = floor(*tile_d / remaining_d);
  double tiles = floor(*other_area_d / *tile_d) / subtiles;
  tiles += 1;
  return (long)(tiles);
}

long get_tiles(
    double *area_w, double *area_h, double *tile_w, double *tile_h,
    int *use_subtiles)
{
  double width_r = fmod(*area_w, *tile_w);
  double height_r = fmod(*area_h, *tile_h);

  // if tiles fit perfectly
  if(width_r == 0 && height_r == 0)
  {
    long x_tiles = *area_w / *tile_w;
    long y_tiles = *area_h / *tile_h;
    long tiles = x_tiles * y_tiles;
    return tiles;
  }

  // if tiles fit one way perfectly
  else if (width_r == 0)
  {
    long tiles = get_tiles_for_one_remaining_dimension(
      area_h, tile_h, area_w, tile_w, use_subtiles);
    return tiles;
  }

  else if (height_r == 0)
  {
    long tiles = get_tiles_for_one_remaining_dimension(
      area_w, tile_w, area_h, tile_h, use_subtiles);
    return tiles;
  }

  // else
  // get tiles for one dimension first
  long tiles = get_tiles_for_one_remaining_dimension(
    area_w, tile_w, area_h, tile_h, use_subtiles);

  // get tiles for remaining dimension
  long extra_remaining_tiles = get_tiles_for_last_dimension(
    area_h, tile_h, area_w, tile_h, use_subtiles);

  tiles += extra_remaining_tiles;
  return tiles;
}


int main(int argc, char *argv[])
{
  // dimensions
  double area_w;
  double area_h;
  double tile_w;
  double tile_h;
  int extra_tiles;      // extra tiles desired
  int tile_cost;    // tile cost in whole numbers
  int subtiles_bool = 0;  // can subtiles be used

  get_user_input(&area_w, &area_h, &tile_w, &tile_h,
    &extra_tiles, &tile_cost, &subtiles_bool);

  // verify inputs
  int is_valid = verify_input(&area_w, &area_h,
    &tile_w, &tile_h, &extra_tiles, &tile_cost);
  while(is_valid == 0)
  {
    printf("One of your inputs is invalid. Please try again.\n");

    get_user_input(&area_w, &area_h, &tile_w, &tile_h,
      &extra_tiles, &tile_cost, &subtiles_bool);

    is_valid = verify_input(&area_w, &area_h,
      &tile_w, &tile_h, &extra_tiles, &tile_cost);
  }

  long tiles = get_tiles(&area_w, &area_h, &tile_w, &tile_h,
    &subtiles_bool);

  long cost = (long)tile_cost * (tiles + extra_tiles);
  double formatted_cost = (double)(cost) / 100;

  printf("Estimated tiles needed: %ld\n", tiles);
  printf("Extra tiles desired: %ld\n", extra_tiles);
  printf("Total tiles needed: %ld\n", tiles + extra_tiles);
  printf("Cost per tile: $%.2f\n", (float)(tile_cost)/100);
  printf("Total estimated cost: $%.2lf\n", formatted_cost);

  return 0;
}
