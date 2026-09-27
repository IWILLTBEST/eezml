Module.simW = 1024; Module.simH = 600;
Module.canvas = document.getElementById('canvas');
Module.canvas.width = Module.simW; Module.canvas.height = Module.simH;
var __g = Module.canvas.getContext('2d');
Module.simImage = __g.createImageData(Module.simW, Module.simH);
Module.simBlit = function(bytes, w, h) {
  if (w !== Module.simW || h !== Module.simH) return;
  var d = Module.simImage.data;
  d.set(bytes);
  // LVGL ARGB8888 memory order is B,G,R,A; ImageData wants R,G,B,A
  for (var i = 0; i < d.length; i += 4) {
    var t = d[i]; d[i] = d[i + 2]; d[i + 2] = t;
  }
  __g.putImageData(Module.simImage, 0, 0);
};
window.shellFit = function() {
  var wrap = document.getElementById('wrap'), c = Module.canvas;
  var s = Math.min((wrap.clientWidth - 16) / c.width,
                   (wrap.clientHeight - 16) / c.height, 1.5);
  c.style.width = Math.round(c.width * s) + 'px';
  c.style.height = Math.round(c.height * s) + 'px';
};
document.getElementById('proj').textContent = document.title;
shellFit(); window.addEventListener('resize', shellFit);
