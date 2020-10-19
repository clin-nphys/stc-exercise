This is an exercise for C++ programming.

 - Compile
To compile you can run the command "g++ anaData.cpp anaTrack.cpp". You can ignore anything
in the ./testcode directory as they are not used in the final product.

 - Method
If a track passes through a point P on the line connecting A and B, where A and B 
are the hit wires, point P divides line segment AB with proportion equal to the distances
from the hit wires to the track.

If this track also passes through other (#hit - 1) division points, it is likely that
this is the track we are looking for. For example, point A is on the left side of the 
track while the 7 rest are at the right side, then the track should pass through (8 - 1)
division points.

But if the track only passes through small number of division points, we should try to 
look for tracks formed by (outer) division points ie points on line AB but not in line 
segment AB with the correct proportion.

The program searches through possible tracks formed from pairs of division points, and
returns the information of a good enough track.

 - Output file
The outfile contains three columns. From left to right is

 | beam angle (deg) | distance from hit to track (um) | drift velocity (um/ns) |

C. Lin, 19 OCT 2020