//
// HAL Programming Contest 2016 Viewer
// Copyright (c) 2016 HAL Laboratory, Inc.
//
'use strict';
var vm = new Vue({
  el: 'body',
  data: {
    json: null, stage: null, turn: null,
    playType: 0, isPlay: false, timer: null, isDrag: false
  },
  computed: {
    stages: function () { return this.json[2]; },
    stageWidth: function () { return this.json[1][1]; },
    stageHeight: function () { return this.json[1][2]; },
    turns: function () { return this.stages[this.stage][2]; },
    ship: function () { return this.turns[this.turn][0]; },
    asteroids: function () { return this.stages[this.stage][1]; },
    lasers: function () { return this.stages[this.stage][3]; },
    shipStyle: function () {
      return { transform: 'translate(' + this.ship[0] + 'px,' + this.ship[1] + 'px)' };
    }
  },
  methods: {
    reset: function (e) {
      e.target.value = null;
    },
    // jsonファイル読み込み関数
    import: function (e) {
      var reader = new FileReader();
      reader.onload = (function () {
        return function (e) {
          try {
            // 読み込んだデータをjsonに保存
            vm.json = JSON.parse(e.target.result);
            vm.stage = 0;
          } catch (ee) {
            alert('JSONファイルが壊れています');
          }
        };
      })(e.target.files[0]);
      reader.readAsText(e.target.files[0]);
    },
    // ターン数に応じて、赤〜緑の色値を計算する関数
    stageColor: function (no) {
      var d = 1 - this.stages[no][2].length/2000;
      d = Math.min(Math.max(d, 0), 1);
      return 'hsl(' + parseInt(d*120) + ', 100%, 50%)';
    },
    isAsteroidExist: function (no) {
      return this.turns[this.turn][1].indexOf(no) >= 0;
    },
    isLaserExist: function (no) {
      var laser = this.lasers[no];
      return laser[0] <= this.turn && this.turn < laser[0] + this.json[1][3];
    },
    // レーザーのDOMに適用するstyleを計算する関数
    laserStyle: function (no) {
      var laser = this.lasers[no], x = laser[1], y = laser[2],
          r = Math.atan2(laser[4] - y, laser[3] - x)*180/Math.PI;
      return {
        opacity: (laser[0] + this.json[1][3] - this.turn)/this.json[1][3],
        transform: 'translate(' + x + 'px,' + y + 'px) rotate(' + r + 'deg)'
      };
    },
    asteroidStyle: function (a) {
      return { transform: 'translate(' + a[0] + 'px,' + a[1] + 'px) scale(' + a[2] + ')' };
    },
    // ターンスライダーをドラッグしている際に呼ばれる関数
    drag: function (e) {
      var sliderRect = e.target.parentNode.getBoundingClientRect();
      this.turn = parseInt((e.clientX - sliderRect.left)*this.turns.length/sliderRect.width);
      this.isDrag = true;
    }
  },
  watch: {
    // ステージが変更されたら、ターンを0にリセット
    stage: function (stage) {
      this.stage = Math.min(Math.max(stage, 0), this.stages.length - 1);
      this.turn = 0;
    },
    // ターンが変更されたら、スライダーも更新
    turn: function (turn) {
      var slider = document.getElementById('slider'),
          control = slider.firstChild.nextSibling;
      this.turn = Math.min(Math.max(turn, 0), this.turns.length - 1);
      control.style.left = parseInt(
        (slider.getBoundingClientRect().width - control.getBoundingClientRect().width)
        *this.turn/this.turns.length
      ) + 'px';
    },
    // 再生ボタンが押されたら、再生状態を変更する
    isPlay: function (isPlay) {
      if (isPlay) {
        this.timer = setTimeout(function loop() {
          if (vm.turn < vm.turns.length - 1) {
            vm.turn += 1;
            vm.timer = setTimeout(loop, 15);
          } else if (vm.playType == 1) {
            vm.turn = 0;
            vm.timer = setTimeout(loop, 15);
          } else if (vm.playType == 2) {
            if (vm.stage < vm.stages.length - 1) {
              vm.stage++;
            } else vm.stage = 0;
            vm.timer = setTimeout(loop, 15);
          } else vm.isPlay = false;
        }, 15);
      } else clearTimeout(this.timer);
    }
  }
});
