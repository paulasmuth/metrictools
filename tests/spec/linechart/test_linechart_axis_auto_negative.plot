width: 1200px;
height: 480px;

plot {
  data: csv('tests/testdata/simple.csv');
  x: $0;
  y: $2;

  layer {
    type: lines;
  }
}
