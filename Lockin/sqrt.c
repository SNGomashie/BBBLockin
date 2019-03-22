
/*
 * Calculate the square root using the iterative babylonian method.
 * This has been tweaked to output an integer anwser
 * https://gist.github.com/foxtrotbravao/2782571
 */

uint16_t sqrt(uint32_t x)
{
	uint32_t fa,sa,ta;
	uint32_t error,error_last;

  // Since the squareroot of X will always be smaller than X / 2
	// We can choose an (arbitrary) first approach as X / 2
	// The closer this number is to the final answer the faster this routines completes.

	fa = x>>1;		// Divide number by 2, bitshifting performs the same operation

	// Divide the argument number by the first approach
	sa = x/fa;

	// Get the mean between the two previous numbers (add them and divide by 2).
	ta = (fa+sa)>>1;

	error_last=-1;
	error=0;

	/*
	 * Repeat this routine until the integer output value is no longer changing.
	 */
	do {
		error_last=error;
		fa = ta;
		sa = x/fa;
		ta = (fa+sa)>>1;
		error=iabs(x - ta*ta);
	} while (error_last!=error);

	// Return the integer result: square root of the input argument x.
	return ta;
}
