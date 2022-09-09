# Hierarchical Layout Blending with Recursive Optimal Correspondence
Qt implementation for our paper "Hierarchical Layout Blending with Recursive Optimal Correspondence" (ACM SIGGRAPH Asia 2022).

We present a novel method for blending hierarchical layouts with semantic
labels. The core of our method is a hierarchical structure correspondence
algorithm, which recursively finds optimal substructure correspondences,
achieving a globally optimal correspondence between a pair of hierarchical
layouts. This correspondence is consistent with the structures of both
layouts, allowing us to define the union of the layouts’ structures. The resulting
compound structure helps extract intermediate layout structures,
from which blended layouts can be generated via an optimization approach.
The correspondence also defines a similarity measure between layouts in a
hierarchically structured view. Our method provides a new way for novel
layout creation. The introduced structural similarity measure regularizes the
layouts in a hyperspace. We demonstrate two applications in this paper, i.e.,
exploratory design of novel layouts and sketch-based layout retrieval, and
test them on a magazine layout dataset. The effectiveness and feasibility of
these two applications are confirmed by the user feedback and the extensive
results. We will release the code of this work upon acceptance of the paper.
