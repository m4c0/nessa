function (obj) {
  const exp = obj.instance.exports;

  function repaint() {
    exp.poc_loop();
    window.requestAnimationFrame(repaint);
  }

  document.body.onclick = function() {
    exp.poc_start();
    window.requestAnimationFrame(repaint);
  };
}

