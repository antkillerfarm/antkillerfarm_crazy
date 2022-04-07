var data = [30, 86, 168, 281, 303, 365];

d3.select(".chart")
  .selectAll("div")
  .data(data)
  .enter()
  .append("div")
  .style("width", function(d) { return d + "px"; })
  .text(function(d) { return d; });

var min = d3.min(data, function(d) {
  return d;
})

var max = d3.max(data, function(d) {
  return d;
})

console.log("min: %d, max: %d, length: %d", min, max, data.length);

// 图表的宽度和高度
var width = 300;
var height = 300;
// 预留给轴线的距离
var padding = { top: 50, right: 50, bottom: 50, left: 50 };

var xScale = d3.scaleBand()
              .domain(d3.range(1, data.length + 1))
              .range([0, width - padding.left - padding.right]);
var yScale = d3.scaleLinear()
              .domain([0, max])
              .range([height - padding.top - padding.bottom, 0]);

var svg = d3.select('body')
            .append('svg')
            .attr('width', width + 'px')
            .attr('height', height + 'px');
var xAxis = d3.axisBottom()
              .scale(xScale);
var yAxis = d3.axisLeft()
              .scale(yScale);

svg.append('g')
  .attr('class', 'axis')
  .attr('transform', 'translate(' + padding.left + ',' + (height - padding.bottom) + ')')
  .call(xAxis);
svg.append('g')
  .attr('class', 'axis')
  .attr('transform', 'translate(' + padding.left + ',' + padding.top + ')')
  .call(yAxis);

var linePath = d3.line()
                .x((d, i) => xScale(i + 1))
                .y(function(d){ return yScale(d) });

svg.append('g')
  .append('path')
  .attr('class', 'line-path')
  .attr('transform', 'translate(' + padding.left + ',' + padding.top + ')')
  .attr('d', linePath(data))
  .attr('fill', 'none')
  .attr('stroke-width', 4)
  .attr('stroke', 'green');

svg.append('g')
  .selectAll('circle')
  .data(data)
  .enter()
  .append('circle')
  .attr('r', 5)
  .attr('transform', function(d, i){
    return 'translate(' + (xScale(i + 1) + padding.left) + ',' + (yScale(d) + padding.top) + ')'
  })
  .attr('fill', 'green');
