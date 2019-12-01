const	barHeight = 20;

data = {}

function renderChart(chartSelector, data, chartWidth) {
	data = data.map(function (value) { return parseInt(value)})
		.filter(Number.isFinite).sort(numbersReverseOrder);

	// X axis scaling function
	let x = d3.scaleLinear()
		.domain([0, d3.max(data)])
		.range([0, chartWidth]);

	// Resize the chart svg
	let chart = d3.select(chartSelector)
		.attr("width", chartWidth)
		.attr("height", barHeight * data.length);

	// Bind data to bars and select bars.
	let bars = chart.selectAll("g")
      .data(data);

	// Create missing bars.
	// Set vertical dimensions.
	let newBars = bars.enter().append("g").attr("transform", function(d, i) { return "translate(0," + i * barHeight + ")"; });
	// Set rectangle height
	newBars.append("rect")
		.attr("height", barHeight - 1);
	// Set text vertical position
	newBars.append("text")
		.attr("y", barHeight / 2)
		.attr("dy", ".35em")
		.text(function(d) { return d; });

	// Set horizontal dimensions for new bars
	// and update for existing bars.
	let allBars = bars.merge(newBars);
	// Set rectangle width
	allBars.selectAll("rect")
		.attr("width", x);
	// Set text position
	allBars.selectAll("text")
		.attr("x", function(d) { return x(d) - 3; })
}

function formatForLinearChart(monthly_commit_count) {
	var parseTime = d3.timeParse("%Y%m%d");

  column_names = monthly_commit_count.map(function(row) {return row.repo_name});
  column_names.unshift('date');

  mapping = {
    'jan_count': '20170101',
    'feb_count': '20170201',
    'mar_count': '20170301',
    'apr_count': '20170401',
    'may_count': '20170501',
    'jun_count': '20170601',
    'jul_count': '20170701',
    'aug_count': '20170801',
    'sep_count': '20170901',
    'oct_count': '20171001',
    'nov_count': '20171101',
    'dec_count': '20171201',
  };

  data_by_date = {
    '20170101': { date: parseTime('20170101')},
    '20170201': { date: parseTime('20170201')},
    '20170301': { date: parseTime('20170301')},
    '20170401': { date: parseTime('20170401')},
    '20170501': { date: parseTime('20170501')},
    '20170601': { date: parseTime('20170601')},
    '20170701': { date: parseTime('20170701')},
    '20170801': { date: parseTime('20170801')},
    '20170901': { date: parseTime('20170901')},
    '20171001': { date: parseTime('20171001')},
    '20171101': { date: parseTime('20171101')},
    '20171201': { date: parseTime('20171201')},
  };
  monthly_commit_count.forEach(function(row) {
    data_by_date[mapping['jan_count']][row.repo_name] = parseInt(row['jan_count']);
    data_by_date[mapping['feb_count']][row.repo_name] = parseInt(row['feb_count']);
    data_by_date[mapping['mar_count']][row.repo_name] = parseInt(row['mar_count']);
    data_by_date[mapping['apr_count']][row.repo_name] = parseInt(row['apr_count']);
    data_by_date[mapping['may_count']][row.repo_name] = parseInt(row['may_count']);
    data_by_date[mapping['jun_count']][row.repo_name] = parseInt(row['jun_count']);
    data_by_date[mapping['jul_count']][row.repo_name] = parseInt(row['jul_count']);
    data_by_date[mapping['aug_count']][row.repo_name] = parseInt(row['aug_count']);
    data_by_date[mapping['sep_count']][row.repo_name] = parseInt(row['sep_count']);
    data_by_date[mapping['oct_count']][row.repo_name] = parseInt(row['oct_count']);
    data_by_date[mapping['nov_count']][row.repo_name] = parseInt(row['nov_count']);
    data_by_date[mapping['dec_count']][row.repo_name] = parseInt(row['dec_count']);
  });
 
  let data = [];
	for (var date in data_by_date) {
			data.push(data_by_date[date]);
	}
  data.columns = column_names;
  return data;
}

function drawLegend(data, y, height, width, chart) {
  const labelHeight = 12;
  let colors = ['#f44336', '#673ab7', '#03a9f4', '#4caf50', '#ffeb3b', '#ff5722', '#607d8b',
	  '#e91e63', '#3f51b5', '#00bcd4', '#8bc34a', '#ffc107', '#795548', '#9c27b0', '#2196f3',
		'#009688', '#cddc39', '#ff9800', '#9e9e9e'];
  
  // Create some nodes
  const labels = []
  for (var key in data[11]) {
    if (key != 'date') {
      labels.push({
				fx: 0,
				targetY: y(data[11][key]),
        project: key,
        count: data[11][key],
      });
    }
  }
  
  // Define a custom force
  const forceClamp = (min, max) => {
    let nodes;
    const force = () => {
      nodes.forEach(n => {
        if (n.y > max) n.y = max;
        if (n.y < min) n.y = min;
      });
    };
    force.initialize = (_) => nodes = _;
    return force;
  }
  
  // Set up the force simulation
  const force = d3.forceSimulation()
    .nodes(labels)
    .force('collide', d3.forceCollide(labelHeight / 2))
    .force('y', d3.forceY(d => d.targetY).strength(1))    
    .force('clamp', forceClamp(0, height))
    .stop();
  
  // Execute the simulation
  for (let i = 0; i < 300; i++) force.tick();
  
  // Assign values to the appropriate marker
  labels.sort((a, b) => a.y - b.y);
  let projects = [];
  for (var key in data[11]) {
    if (key != 'date') {
      datum = { name: key, value: data[11][key]};
			let color = colors.shift();
			datum.color = color;
			colors.push(color);
      projects.push(datum);
    }
  }
  projects.sort((a, b) => b.value - a.value);
  projects.forEach((project, i) => {
	  project.y = labels[i].y;
		project.targetY = labels[i].targetY;
	});
  
  // Add labels
  const legendItems = chart.selectAll('.legend-item').data(projects, p => p.name);
  legendItems.exit().remove();
  legendItems.attr('y', d => d.y);
  legendItems.enter().append('text')
    .attr('class', 'legend-item')
    .html(d => d.name)
    .attr('fill', d => d.color)
		.style("font", "12px sans-serif")
    .attr('x', width)
    .attr('dx', '4.5em')
		.attr("dy", "2em")
    .attr('y', d => d.y);
}

function renderMultiSeriesLineChart(monthly_commit_count) {
  let svg = d3.select("#monthly-commit-count");
  let margin = {top: 20, right: 120, bottom: 30, left: 50};
  let width = svg.attr("width") - margin.left - margin.right;
  let height = svg.attr("height") - margin.top - margin.bottom;
	// material design colors http://materializecss.com/color.html
  let colors = ['#f44336', '#673ab7', '#03a9f4', '#4caf50', '#ffeb3b', '#ff5722', '#607d8b',
	  '#e91e63', '#3f51b5', '#00bcd4', '#8bc34a', '#ffc107', '#795548', '#9c27b0', '#2196f3',
		'#009688', '#cddc39', '#ff9800', '#9e9e9e'];
  g = svg.append("g").attr("transform", "translate(" + margin.left + "," + margin.top + ")");

  let x = d3.scaleTime().range([0, width]);
	let y = d3.scaleLinear().range([height, 0]);

  var line = d3.line()
    .curve(d3.curveBasis)
    .x(function(d) { return x(d.date); })
    .y(function(d) { return y(d.count); });

	// TODO: Pack data into CSV instead of JSON for compactness
	// e.g. d3.tsv("data.tsv"...)

  var data = formatForLinearChart(monthly_commit_count);
	var repos = data.columns.slice(1).map(function(id) {
		return {
			id: id,
			values: data.map(function(d) {
				return {date: d.date, count: d[id]};
			})
		};
	});

  x.domain(d3.extent(data, function(d) { return d.date; }));

  y.domain([
    d3.min(repos, function(c) { return d3.min(c.values, function(d) { return d.count; }); }),
    d3.max(repos, function(c) { return d3.max(c.values, function(d) { return d.count; }); })
  ]);

  g.append("g")
      .attr("class", "axis axis--x")
      .attr("transform", "translate(0," + height + ")")
      .call(d3.axisBottom(x));

  g.append("g")
      .attr("class", "axis axis--y")
      .call(d3.axisLeft(y))
    .append("text")
      .attr("transform", "rotate(-90)")
      .attr("y", 6)
      .attr("dy", "0.71em")
      .attr("fill", "#000")
      .text("Monthly commit count");

  var repo = g.selectAll(".repo")
    .data(repos)
    .enter().append("g")
      .attr("class", "repo");

  repo.append("path")
      .attr("class", "line")
      .attr("d", function(d) { return line(d.values); })
      .style("stroke", function(d) {
         let color = colors.shift();
         d.c = color;
         colors.push(color);
         return color;
         //return z(d.id);
      });

  drawLegend(data, y, height, width, svg);
}

function renderCharts() {

	let star_count = data['star_count'].map(function (row) {
		return row['count'];
	});

	let contributor_count = data['contributor_count'].map(function (row) {
		return row['count'];
	});

	let commit_count = data['commit_count'].map(function (row) {
		return row['count'];
	});

	let question_count = data['question_count'].map(function (row) {
		return row['count'];
	});

	// Read bar chart responsive width.
	// To keep bar chart responsive we do not set it width in pixels here.
	// Instead its width is set in CSS relative to the parent container width.
	// All other char elements are sized relative to bar chart width.
	var chartWidth = parseInt(d3.select('.chart').style('width'));

	renderChart("#chart-star-count", star_count, chartWidth);
	renderChart("#chart-contributor-count", contributor_count, chartWidth);
	renderChart("#chart-commit-count", commit_count, chartWidth);
	renderChart("#chart-question-count", question_count, chartWidth);

	let monthly_commit_count = data['monthly_commit_count']

	renderMultiSeriesLineChart(monthly_commit_count);
}

d3.select(window).on('resize', function() {
  resize();
}); 

function resize() {
	renderCharts();
}

/**********  UTILITY FUNCTIONS ************/

/*
 * Utility function to sort integer arrays
 */
function numbersReverseOrder(a, b) {
  return b - a;
}

$(function() {
	topic_id = location.href.substr(location.href.lastIndexOf('/') + 1);
	if (!topic_id.trim()) {
	  topic_id = 'top';
	}
	$.get("/topics/" + topic_id + "/metrics", function(response) {
		data = response.metrics;
		renderCharts();
	});
});
