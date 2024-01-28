We used ConnectedThresholdImageFilter, a region growing filter. For an explanation of how this filter works,
see InsightSoftware Guide book 2, page 340. 

For smoothing the image, we used CurvatureFlowImageFilter (same book, page 109). To label and count the regions, we used 
ConnectedComponentImageFilter. 

Here's the order:

1. CurvatureFlowImageFilter
2. ConnectedThresholdImageFilter
3. use doOpening() on the result of ConnectedThresholdImageFilter
4. ConnectedComponentImageFilter

****The filter in step 2 and the filter in step 4 are different, and we used them for different things****
