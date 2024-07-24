# SDTVer050.0 G0ORX Version

This version has the Bearing and BodePlotter code included in the compile.

Latest SDTVer050.0 with G0ORX MCP23017 Front Panel Code and Kenwood TS-2000 CAT interface.

MyConfigurationFile.h has define of G0ORX_FRONTPANEL and G0ORX_CAT.

Note that this MCP23017 Front Panel Code support the K9HZ Version of the Front Panel and encoders.

I have also made some other small changes to get some small changes to increase the stack memory.

To build this version configure the IDE as:

Tools->Optimize->Faster with LTO

Tools->USB Type->Dual Serial

The memory Usage with these options set and both G0ORX_FRONTPANEL and G0ORX_CAT enabled now looks like:

```
   FLASH: code:274516, data:93624, headers:8684   free for files:774964
   RAM1: variables:200864, code:256936, padding:5208   free for local variables:61280
   RAM2: variables:454272  free for malloc/new:70016
```

