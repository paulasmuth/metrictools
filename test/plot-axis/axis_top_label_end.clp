(set-height 60px)
(layout/add-margins margin 1em)

(plot/draw-axis
    align top
    limit (1451606400 1451610000)
    label-placement (subdivide 4)
    label-format (datetime "%H:%M:%S")
    label-attach right
    label-rotate 90)