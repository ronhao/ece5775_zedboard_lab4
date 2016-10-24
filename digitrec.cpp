//==========================================================================
//digitrec.cpp
//==========================================================================
// @brief: A k-nearest-neighbor implementation for digit recognition (k=1)

#include "digitrec.h"

//----------------------------------------------------------
// Top function
//----------------------------------------------------------

void dut(
    hls::stream<bit32_t> &strm_in,
    hls::stream<bit32_t> &strm_out
)
{
  // -----------------------------
  // YOUR CODE GOES HERE
  // -----------------------------
  digit input;
  bit4_t output;

  bit32_t input_lo = strm_in.read();
  bit32_t input_hi = strm_in.read();

  input(31, 0) = input_lo;
  input(input.length()-1, 32) = input_hi;

  output = digitrec( input );

  strm_out.write( output );
}

//----------------------------------------------------------
// Digitrec
//----------------------------------------------------------
// @param[in] : input - the testing instance
// @return : the recognized digit (0~9)

bit4_t digitrec( digit input ) 
{
  #include "training_data.h"

  // This array stores K minimum distances per training set
  bit6_t knn_set[10][K_CONST];

  // Initialize the knn set
  for ( int i = 0; i < 10; ++i )
    for ( int k = 0; k < K_CONST; ++k )
      // Note that the max distance is 49
      knn_set[i][k] = 50; 

  L1800: for ( int i = 0; i < TRAINING_SIZE; ++i ) {
    L10: for ( int j = 0; j < 10; j++ ) {
      // Read a new instance from the training set
      digit training_instance = training_data[j][i];
      // Update the KNN set
      update_knn( input, training_instance, knn_set[j] );
    }
  } 

  // Compute the final output
  return knn_vote( knn_set ); 
}


//-----------------------------------------------------------------------
// update_knn function
//-----------------------------------------------------------------------
// Given the test instance and a (new) training instance, this
// function maintains/updates an array of K minimum
// distances per training set.

// @param[in] : test_inst - the testing instance
// @param[in] : train_inst - the training instance
// @param[in/out] : min_distances[K_CONST] - the array that stores the current
//                  K_CONST minimum distance values per training set

void update_knn( digit test_inst, digit train_inst, bit6_t min_distances[K_CONST] )
{
  // Compute the difference using XOR
  digit diff = test_inst ^ train_inst;

  bit6_t dist = 0;
  // Count the number of set bits
  for ( int i = 0; i < 49; ++i ) { 
    dist += diff[i];
  }

  // -----------------------------
  // YOUR CODE GOES HERE
  // -----------------------------
  // can do this given the fact that K_CONST is 3
  if (dist <= min_distances[2]) {
    if (dist <= min_distances[1]) {
      if (dist <= min_distances[0]) {
        min_distances[2] = min_distances[1];
        min_distances[1] = min_distances[0];
        min_distances[0] = dist;
      }
      else {
        min_distances[2] = min_distances[1];
        min_distances[1] = dist;
      }
    }
    else min_distances[2] = dist;
  }
  /*
  bit6 temp_distances[K_CONST+1];
  //#pragma HLS ARRAY_PARTITION variable=temp_distances dim=0
  bit6 temp_distance;

  for (int i = 0; i < K_CONST; i++) {
    //#pragma HLS unroll
    temp_distances[i] = min_distances[i];
  }
  temp_distances[K_CONST] = dist;

  for (int i = K_CONST; i >= 1; i--) {
    if (temp_distances[i] < temp_distances[i-1]) {
      temp_distance = temp_distances[i];
      temp_distances[i] = temp_distances[i-1];
      temp_distances[i-1] = temp_distance;
    }
  }

  for(int i = 0; i < K_CONST; i++) {
    //#pragma HLS unroll
    min_distances[i] = temp_distances[i];
  }
  */
}


//-----------------------------------------------------------------------
// knn_vote function
//-----------------------------------------------------------------------
// Given 10xK minimum distance values, this function 
// finds the actual K nearest neighbors and determines the
// final output based on the most common digit represented by 
// these nearest neighbors (i.e., a vote among KNNs). 
//
// @param[in] : knn_set - 10xK_CONST min distance values
// @return : the recognized digit
// 

bit4_t knn_vote( bit6_t knn_set[10][K_CONST] )
{
  // -----------------------------
  // YOUR CODE GOES HERE
  // -----------------------------
  int result;
  int sum_of_dis = 0;
  int min_sum_of_dis = K_CONST*50;
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < K_CONST; j++) {
      sum_of_dis += knn_set[i][j];
      
    }
    // find the digit (1~10) with the minimum sum of distances
    if (sum_of_dis < min_sum_of_dis) {
      result = i;
      min_sum_of_dis = sum_of_dis;
    }
    sum_of_dis = 0;
  }
  return result;
}

