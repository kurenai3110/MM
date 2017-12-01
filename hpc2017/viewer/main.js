//
// HAL Programming Contest 2017 Viewer
// Copyright (c) 2017 HAL Laboratory, Inc.
//
'use strict';
const vm = new Vue({
  el: 'body',
  data: {
    json: true, stage: 0, turn: 0,
    playType: 0, isPlay: false, timer: null, isDrag: false,
    balloon: null,
    minColorScore: 50, maxColorScore: 150
  },
  created: function () {
    if (location.href.match(/(?:\?|&)data=/)) this.importparam();
  },
  ready: function () {
    setInterval(function () {
      var dom = document.querySelector('.tooltip:hover');
      vm.balloon = dom?{
        left: dom.style.left, top: dom.style.top,
        text: dom.dataset['tooltip'] }:null;
    }, 100);
  },
  computed: {
    totalturn: function () { return this.json[0]; },
    configs: function () { return this.json[1]; },
    ufoConfigs: function () { return this.configs[5]; },
    stages: function () { return this.json[2]; },
    office: function () { return this.stages[this.stage][1][0]; },
    houses: function () { return this.stages[this.stage][1][1]; },
    turns: function ()  { return this.stages[this.stage][2]; },
    ufos: function () { return this.turns[this.turn][0]; }
  },
  methods: {
    getParameterByName: function(name) {
      name = name.replace(/[\[]/, "\\\[").replace(/[\]]/, "\\\]");
      var regex = new RegExp("[\\?&]" + name + "=([^&#]*)"),
          results = regex.exec(location.search);
      return results == null ? "" : decodeURIComponent(results[1].replace(/\+/g, " "));
    },
    // <input type="file">からjsonファイルが選択された時に呼ばれる
    import: function (e) {
      const reader = new FileReader();
      reader.onload = (function () {
        return function (e) {
          try {
            vm.json = JSON.parse(e.target.result);
            vm.stage = 0;
          } catch (ee) {
            alert('JSONファイルが壊れています');
          }
        }
      })(e.target.files[0]);
      reader.readAsText(e.target.files[0]);
    },
    // urlの引数で data=[jsonファイルのパス] が指定されている時に呼ばれる
    importparam: function (e) {
      var xhr = new XMLHttpRequest();
      xhr.open('GET', this.getParameterByName('data'));
      xhr.onreadystatechange = function () {
        if (xhr.readyState === 4) {
          if (xhr.status === 200 || xhr.status === 0) {
            try {
              vm.json = JSON.parse(xhr.responseText);
              vm.stage = 0;
            } catch (ee) {
              alert('JSONファイルが壊れています');
            }
          } else if(xhr.status === 404) {
            alert('JSONファイルが見つかりません');
          }
        }
      };
      xhr.send(null);
    },
    drag: function (e) {
      const sliderRect = e.target.parentNode.getBoundingClientRect();
      this.turn = parseInt((e.clientX - sliderRect.left)*this.turns.length/sliderRect.width);
      this.isDrag = true;
    },
    delivered: function (no) {
      return !!((this.turns[this.turn][1][parseInt(no/32)] >>> (no%32)) & 1);
    },
    stageColor: function (no) {
      var turn = this.stages[no][2].length,
          weight = (turn - this.maxColorScore)/(this.minColorScore - this.maxColorScore);
      weight = Math.min(Math.max(weight, 0), 1);
      return 'hsl(' + parseInt(weight*120) + ', 100%, 50%)';
    },
    circleStyle: function (x, y, r) {
      return { borderRadius: r + 'px', left: x + 'px', top: y + 'px', padding: r + 'px' };
    },
    officeStyle: function () {
      return this.circleStyle(this.office[0], this.office[1], this.configs[3]);
    },
    houseStyle: function (no) {
      var house = this.houses[no],
          style = this.circleStyle(house[0], house[1], this.configs[4]);
      style.background = this.delivered(no)?'#c74':'#874';
      return style;
    },
    ufoStyle: function (no) {
      var ufo = this.ufos[no];
      return this.circleStyle(ufo[0][0], ufo[0][1], this.ufoConfigs[ufo[1]][0]);
    },
    balloonStyle: function () {
      return { left: this.balloon.left, top: this.balloon.top };
    }
  },
  watch: {
    stage: function (stage) {
      this.stage = Math.min(Math.max(stage, 0), this.stages.length - 1);
      this.turn = 0;
    },
    turn: function (turn) {
      const slider = document.getElementById('slider'),
            control = slider.firstChild.nextSibling;
      this.turn = Math.min(Math.max(turn, 0), this.turns.length - 1);
      control.style.left = parseInt(
        (slider.getBoundingClientRect().width - control.getBoundingClientRect().width)
        *this.turn/this.turns.length
      ) + 'px';
    },
    isPlay: function (isPlay) {
      var frame = 50;
      if (isPlay) {
        this.timer = setTimeout(function loop() {
          if (vm.turn < vm.turns.length - 1) {
            vm.turn++;
            vm.timer = setTimeout(loop, frame);
          } else if (vm.playType == 1) {
            vm.turn = 0;
            vm.timer = setTimeout(loop, frame);
          } else if(vm.playType == 2) {
            if (vm.stage < vm.stages.length - 1) {
              vm.stage++
            } else vm.stage = 0;
            vm.timer = setTimeout(loop, frame);
          } else vm.isPlay = false;
        }, frame);
      } else clearTimeout(this.timer);
    }
  }
})
