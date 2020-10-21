This is an exercise for C++ programming.

 - Compile
To compile you can run the command

  g++ anaData.cpp anaTrack.cpp `root-config --cflags --libs`

You can ignore anything in the ./testcode directory asthey are not used in the
final product.

 - Method
If a track passes through a point P on the line connecting A and B, where A and B 
are the hit wires, point P divides line segment AB with proportion equal to the
distances from the hit wires to the track.

If this track also passes through other (#hit - 1) division points, it is likely that
this is the track we are looking for. For example, point A is on the left side of the 
track while the 7 others are at the right side, then the track should pass through (8 - 1)
division points.

But if the track only passes through small number of division points, we should try to 
look for tracks formed by (outer) division points ie points on line AB but not in line 
segment AB with the correct proportion. (This feature was removed since it does not
adequately improve the fit.)

The program searches through possible tracks formed from pairs of division points, and
returns the track slope and y-interception. With the information, the average drift
velocity and the average beam angle are calculated.

 - Output file
The outfile contains two columns. From left to right is

 |  beam angle (deg)  |  drift velocity (um/ns)  |

C. Lin, 21 OCT 2020