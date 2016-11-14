

var squall_meta = [
  'Brads Pocket',
  'Next to Amit',
  'Branden lost this one',
  'Gravy',
  'Playing pingpong'
];

var squall_history = [];

// current squall to graph
var active_squall_id = null;

var app = {
  // Application Constructor
  initialize: function() {
    document.addEventListener("deviceready", app.onAppReady, false);
    document.addEventListener("resume", app.onAppReady, false);
    document.addEventListener("pause", app.onPause, false);
    document.addEventListener("data", app.onData, false);

    $("#sign").on("click", ".squall", app.onClick);
    $("#sign").on("click", "#closegraph", app.onCloseGraph);

    // Init Graph
    var svg = d3.select("#chart");
    var margin = {top: 20, right: 80, bottom: 30, left: 50};
    var width = 600;
    var height = 220;
    var g = svg.append("g")
        .attr("class", "maing")
        .attr("transform", "translate(" + margin.left + "," + margin.top + ")");


    if (typeof summon == "undefined") {

      // setInterval(function () {
      //   function getRandomInt(min, max) {
      //     return Math.floor(Math.random() * (max - min + 1)) + min;
      //   }
      //   var arr = [
      //     getRandomInt(0, 4),
      //     getRandomInt(0, 4),
      //     getRandomInt(0, 4),
      //     getRandomInt(0, 4),
      //     getRandomInt(0, 4),
      //   ];

      //   document.dispatchEvent(new CustomEvent("data", {detail: arr}));
      // }, 1000);


      // if (!SIMULATE_PACKETS) {
        var client = mqtt.connect("ws://imix.j2x.us:9001/mqtt");
        client.on("connect", function () {
          client.subscribe('device/imix/0026e1690001');
        });
        client.on("message", function(topic, payload) {
          var data = JSON.parse(payload.toString());
          // SIMULATE_PACKETS=true;
          var arr = [
            data.squall0,
            data.squall1,
            data.squall2,
            data.squall3,
            data.squall4
          ];
          document.dispatchEvent(new CustomEvent("data",{detail:arr}));
        });
      //   setTimeout(function(){if(!SIMULATE_PACKETS)app.simulatePackets()},30000); // simulate if nothing happens in 30s
      // } else app.simulatePackets();
    } else {
      app.log("Running Imix/Tock Demo UI on Summon");
    }
  },

  // App Ready Event Handler
  onAppReady: function() {
    app.log("onAppReady");
    app.log("Checking if BLE is enabled...");
    summon.bluetooth.isEnabled(app.onEnable);
  },
  // App Paused Event Handler
  onPause: function() {
    app.log("Pause");
    summon.bluetooth.stopScan();
  },
  // New Data Received Event Handler
  onData: function(data) {
    // console.log(data.detail);
    var squall_to_imix = {
      '0': data.detail[0],
      '1': data.detail[1],
      '2': data.detail[2],
      '3': data.detail[3],
      '4': data.detail[4],
    };

    // Add to history
    squall_history.push({
      '0': data.detail[0],
      '1': data.detail[1],
      '2': data.detail[2],
      '3': data.detail[3],
      '4': data.detail[4],
      'timestamp': new Date()
    });

    // Remove history greater than a value
    while (squall_history.length > 20) {
      squall_history.splice(0, 1);
    }


    // Keep track of the number of squalls at each imix
    var attached_squalls = [0, 0, 0, 0, 0];

    var positions = [
      {x: 660, y: 330},
      {x: 400, y: 180},
      {x: 980, y: 180},
      {x: 400, y: 480},
      {x: 980, y: 480},
    ];
    var name_offset_x = 20;
    var name_offset_y = 8;


    for (var squall_id=0; squall_id<5; squall_id++) {

      var imix_id = squall_to_imix[squall_id];
      if (imix_id <= 4) {
        var index = attached_squalls[imix_id];
        var x = positions[imix_id].x;
        var y = positions[imix_id].y + (index * 35);
        var x_name = x + name_offset_x;
        var y_name = y + name_offset_y;

        // Update main UI
        $('#squall' + squall_id).attr('x', x);
        $('#squall' + squall_id).attr('y', y);
        $('#squall'+squall_id+'_name').attr('x', x_name);
        $('#squall'+squall_id+'_name').attr('y', y_name);
        $('#squall'+squall_id+'_name').text(squall_meta[squall_id]);

        attached_squalls[imix_id] += 1;
      }
    }



    if (active_squall_id == null) {
      return;
    }


    var svg = d3.select("#chart");
    var margin = {top: 20, right: 80, bottom: 30, left: 50};
    var width = 600;
    var height = 220;

    var g = svg.selectAll(".maing");

    var x = d3.scaleTime().range([0, width]);
    var y = d3.scaleLinear().range([height, 0]);

    x.domain(d3.extent(squall_history, function(d) { return d.timestamp; }));
    y.domain([0, 4]);

    var xaxis = g.selectAll(".axisx")
      .data([0])
      .enter()
        .append("g")
          .attr("class", "axis axisx")
          .attr("transform", "translate(0," + height + ")")
        .append("text")
          .attr("transform", "rotate(-90)")
          .attr("y", -30)
          .attr("x", 110)
          // .attr("dy", "2.71em")
          .attr("fill", "#000")
          .attr("class", "ylabel")
          .text("Imix ID");


    svg.selectAll("g.axisx").call(d3.axisBottom().scale(x).tickFormat(d3.timeFormat("%H:%M:%S")).ticks(5));


    var yaxis = g.selectAll("axis--y")
      .data([0])
      .enter()
        .append("g")
        .attr("class", "axis axis--y");

    yaxis.call(d3.axisLeft(y).tickFormat(d3.format("d")).ticks(5));

    var line = d3.line()
      .x(function(d) { return x(d.timestamp); })
      .y(function(d) { return y(d[active_squall_id]); });



    // Create line if not exist
    var lines = g.selectAll(".line")
      .data([0])
      .enter()
        .append("path")
        .attr("class", "line");

    // Update line
    svg.selectAll("path.line").attr("d", line(squall_history));



    g.selectAll(".dot")
      .data(squall_history)
    .enter().append("circle")
      .attr("class", "dot")
      .attr("r", 10)
      .attr("cx", function(d) { return x(d.timestamp); })
      .attr("cy", function(d) { return y(d[active_squall_id]); });

    svg.selectAll("g circle.dot")
      .data(squall_history)
      .attr("r", 10)
      .attr("cx", function(d) { return x(d.timestamp); })
      .attr("cy", function(d) { return y(d[active_squall_id]); });


  },
  // Bluetooth Enabled Callback
  onEnable: function() {
    app.onPause();
    summon.bluetooth.startScan([], app.onDiscover, app.onAppReady);
    app.log("Searching");
  },
  // BLE Device Discovered Callback
  onDiscover: function(device) {
    // app.log("Found " + device.name + " (" + device.id + ")!");
    // var advertisement = device.advertisement;
    // if (device.id == gateway.getDeviceId() && advertisement.manufacturerData.length) {
    //   var data = null;
    //   var md = advertisement.manufacturerData;
    //   switch (md[0] * 0x100 + md[1]) {
    //     case 0x2001: // Power Supply
    //       data = {
    //         device: "signpost_status",
    //         module0_energy_mAh: md[2] * 0x100 + md[3],
    //         module1_energy_mAh: md[4] * 0x100 + md[5],
    //         module2_energy_mAh: md[6] * 0x100 + md[7],
    //         controller_energy_mAh: md[8] * 0x100 +md[9],
    //         module5_energy_mAh: md[12] * 0x100 + md[13],
    //         module6_energy_mAh: md[14] * 0x100 + md[15],
    //         module7_energy_mAh: md[16] * 0x100 + md[17],
    //         module0_enabled: Boolean(md[18] & 0b10000000),
    //         module1_enabled: Boolean(md[18] & 0b01000000),
    //         module2_enabled: Boolean(md[18] & 0b00100000),
    //         module5_enabled: Boolean(md[18] & 0b00000100),
    //         module6_enabled: Boolean(md[18] & 0b00000010),
    //         module7_enabled: Boolean(md[18] & 0b00000001),
    //       }; break;
    //     case 0x2002: // Controller
    //       data = {
    //         device: "signpost_gps",
    //         latitude: Math.abs((md[8]*0x1000000+md[9]*0x10000+md[10]*0x100+md[11])/(10000*100.0)),
    //         latitude_direction: md[8]&0b10000000 ? "S" : "N",
    //         longitude: Math.abs((md[12]*0x1000000+md[13]*0x10000+md[14]*0x100+md[15])/(10000*100.0)),
    //         longitude_direction: md[12]&0b10000000 ? "W" : "E",
    //         timestamp: new Date(Date.UTC(md[4]+2000, md[3]-1, md[2], md[5], md[6], md[7])).toISOString(),
    //       }; break;
    //     case 0x2201: // Radio
    //       data = {
    //         device: "signpost_radio_status",
    //         "status_ble_packets_sent": Number(((md[2]*0x100+md[3])*(7.0/8.0)).toFixed(0)),
    //         "gps_ble_packets_sent": Number(((md[4]*0x100+md[5])*(7.0/8.0)).toFixed(0)),
    //         "2.4gHz_spectrum_ble_packets_sent":Number(((md[6]*0x100+md[7])*(7.0/8.0)).toFixed(0)),
    //         "ambient_sensing_ble_packets_sent":Number(((md[8]*0x100+md[9])*(7.0/8.0)).toFixed(0)),
    //         "audio_spectrum_ble_packets_sent": Number(((md[10]*0x100+md[11])*(7.0/8.0)).toFixed(0)),
    //         "microwave_radar_ble_packets_sent":Number(((md[12]*0x100+md[13])*(7.0/8.0)).toFixed(0)),
    //         "ucsd_air_quality_ble_packets_sent":Number(((md[14]*0x100+md[15])*(7.0/8.0)).toFixed(0)),
    //         "status_lora_packets_sent": (md[2]*0x100+md[3]),
    //         "gps_lora_packets_sent": (md[4]*0x100+md[5]),
    //         "2.4gHz_spectrum_lora_packets_sent": (md[6]*0x100+md[7]),
    //         "ambient_sensing_lora_packets_sent": (md[8]*0x100+md[9]),
    //         "audio_spectrum_lora_packets_sent": (md[10]*0x100+md[11]),
    //         "microwave_radar_lora_packets_sent": (md[12]*0x100+md[13]),
    //         "ucsd_air_quality_lora_packets_sent": (md[14]*0x100+md[15]),
    //         "status_radio_energy_used_mWh": Number((md[2]*0x100+md[3])*(0.000096+0.01)).toFixed(3),
    //         "gps_radio_energy_used_mWh": Number((md[4]*0x100+md[5])*(0.000096+0.01)).toFixed(3),
    //         "2.4gHz_spectrum_radio_energy_used_mWh": Number((md[6]*0x100+md[7])*(0.000096+0.01)).toFixed(3),
    //         "ambient_sensing_radio_energy_used_mWh": Number((md[8]*0x100+md[9])*(0.000096+0.01)).toFixed(3),
    //         "audio_spectrum_radio_energy_used_mWh": Number((md[10]*0x100+md[11])*(0.000096+0.01)).toFixed(3),
    //         "microwave_radar_radio_energy_used_mWh": Number((md[12]*0x100+md[13])*(0.000096+0.01)).toFixed(3),
    //         "ucsd_air_quality_radio_energy_used_mWh": Number((md[14]*0x100+md[15])*(0.000096+0.01)).toFixed(3),
    //       }; break;
    //     case 0x3101: // 2.4GHz Spectrum
    //       var md8 = new Int8Array(md.buffer.slice(2,18));
    //       data = {
    //         device: 'signpost_2.4ghz_spectrum',
    //         channel_11: md8[0],
    //         channel_12: md8[1],
    //         channel_13: md8[2],
    //         channel_14: md8[3],
    //         channel_15: md8[4],
    //         channel_16: md8[5],
    //         channel_17: md8[6],
    //         channel_18: md8[7],
    //         channel_19: md8[8],
    //         channel_20: md8[9],
    //         channel_21: md8[10],
    //         channel_22: md8[11],
    //         channel_23: md8[12],
    //         channel_24: md8[13],
    //         channel_25: md8[14],
    //         channel_26: md8[15],
    //       }; break;
    //     case 0x3201: // Ambient
    //       data = {
    //         device: 'signpost_ambient',
    //         temperature_c: (md[2] * 0x100 + md[3]) / 100.0,
    //         humidity: (md[4] * 0x100 + md[5]) / 100.0,
    //         light_lux: md[6] * 0x100 + md[7],
    //         pressure_pascals: md[8] * 0x100 +md[9],
    //       }; break;
    //     case 0x3301: // Audio Frequency
    //       data = {
    //         device: 'signpost_audio_spectrum',
    //         "63Hz": Number(((Math.log10((md[2]*0x100+md[3])/43.75)*20)+35.5).toFixed(0)),
    //         "160Hz": Number(((Math.log10((md[4]*0x100+md[5])/43.75)*20)+35.5).toFixed(0)),
    //         "400Hz": Number(((Math.log10((md[6]*0x100+md[7])/43.75)*20)+35.5).toFixed(0)),
    //         "1000Hz": Number(((Math.log10((md[8]*0x100+md[9])/43.75)*20)+35.5).toFixed(0)),
    //         "2500Hz": Number(((Math.log10((md[10]*0x100+md[11])/43.75)*20)+35.5).toFixed(0)),
    //         "6250Hz": Number(((Math.log10((md[12]*0x100+md[13])/43.75)*20)+35.5).toFixed(0)),
    //         "16000Hz": Number(((Math.log10((md[14]*0x100+md[15])/43.75)*20)+35.5).toFixed(0)),
    //       }; break;
    //     case 0x3401: // Microwave Radar
    //       data = {
    //         device: 'signpost_microwave_radar',
    //         motion: md[2] & 0b10000000,
    //         'velocity_m/s': (md[2]*0x1000000+md[3]*0x10000+md[4]*0x100+md[5]) / 1000.0,
    //       }; break;
    //     case 0x3501: // Air Quality
    //       data = {
    //         device: 'signpost_ucsd_air_quality',
    //         co2_ppm: md[2] * 0x100 + md[3],
    //         VOC_PID_ppb: md[4]*0x1000000+md[5]*0x10000+md[6]*0x100+md[7],
    //         VOC_IAQ_ppb: md[8]*0x1000000+md[9]*0x10000+md[10]*0x100+md[11],
    //         barometric_millibar: md[12] * 0x100 + md[13],
    //         humidity_percent: md[14] * 0x100 + md[15],
    //       }; break;
    //   }
    //   if (data) document.dispatchEvent(new CustomEvent("data",{detail:data}));
    // }
  },
  // Module Click Event Handler
  onClick: function() {
    var id = $(this).attr('id');
    active_squall_id = parseInt(id.substr(6, 1));
    console.log(active_squall_id)

    $(".mod b").text("Squall " + active_squall_id);
    $("#chartfo").attr("x", 200);
    $("#chartfo").attr("y", 200);
  },
  onCloseGraph: function() {
    active_squall_id = null;
    $("#chartfo").attr("x", 4000);
    $("#chartfo").attr("y", 4000);
  },
  // Function to simulate data packets
  simulatePackets: function() {
    d3.select("#sim").attr("class","flash")
    simulatePackets();
  },
  // Function to Log Text to Screen
  log: function(string) {
    console.log(string);
  },
  x:{},
  y:{}
};

app.initialize();