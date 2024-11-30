window.addEventListener("load", (event) => {
  // Expand Left Side
  var icon = document.querySelector(".hamb"),
    left = document.querySelector(".left");

  icon.addEventListener("click", expand);

  function expand() {
    if (left.classList.contains("show")) {
      left.classList.remove("show");
    } else {
      left.classList.add("show");
    }
  }

  // Toggles
  var toggle = document.querySelectorAll(".toggle");

  toggle.forEach(function (el) {
    el.addEventListener("click", activateToggle);
  });

  function activateToggle(event) {
    var currentToggle = event.target;

    if (currentToggle.classList.contains("off")) {
      currentToggle.classList.remove("off");
    } else {
      currentToggle.classList.add("off");
    }
  }

  // Font Size Options
  var letter = document.querySelectorAll(".letter"),
    sizeS = document.querySelector(".size_s"),
    sizeM = document.querySelector(".size_m"),
    sizeL = document.querySelector(".size_l"),
    container = document.querySelector(".container");

  letter.forEach(function (el) {
    el.addEventListener("click", activateLetter);
  });

  function activateLetter(event) {
    var currentLetter = event.currentTarget,
      allLetters = document.querySelectorAll(".letter");

    allLetters.forEach(function (el) {
      el.classList.remove("select");
    });
    currentLetter.classList.add("select");

    if (sizeS.classList.contains("select")) {
      container.setAttribute("data-size", "small");
    }

    if (sizeM.classList.contains("select")) {
      container.setAttribute("data-size", "");
    }
    if (sizeL.classList.contains("select")) {
      container.setAttribute("data-size", "large");
    }
  }

  // Themes Options
  var color = document.querySelectorAll(".color"),
    colorPurple = document.querySelector(".c_purple"),
    colorGreen = document.querySelector(".c_green"),
    colorBlue = document.querySelector(".c_blue"),
    colorPink = document.querySelector(".c_pink"),
    colorOrange = document.querySelector(".c_orange");

  color.forEach(function (el) {
    el.addEventListener("click", changeTheme);
  });

  function changeTheme(event) {
    var currentColor = event.target,
      allColors = document.querySelectorAll(".color");

    allColors.forEach(function (el) {
      el.classList.remove("active_color");
    });
    currentColor.classList.add("active_color");

    if (colorPurple.classList.contains("active_color")) {
      container.setAttribute("data-theme", "");
    }

    if (colorGreen.classList.contains("active_color")) {
      container.setAttribute("data-theme", "green");
    }
    if (colorBlue.classList.contains("active_color")) {
      container.setAttribute("data-theme", "blue");
    }
    if (colorPink.classList.contains("active_color")) {
      container.setAttribute("data-theme", "pink");
    }
    if (colorOrange.classList.contains("active_color")) {
      container.setAttribute("data-theme", "orange");
    }
  }
});
function switchTab(tabId) {
  // Ẩn tất cả các tab nội dung
  const tabs = document.querySelectorAll(".tab_content");
  tabs.forEach((tab) => tab.classList.remove("active"));

  // Hiển thị tab được chọn
  document.getElementById(tabId).classList.add("active");

  // Xóa trạng thái active khỏi tất cả liên kết
  const links = document.querySelectorAll(".side_link");
  links.forEach((link) => link.classList.remove("active"));

  // Đặt trạng thái active cho liên kết được chọn
  const activeLink = document.getElementById(`${tabId}_tab`);
  if (activeLink) activeLink.classList.add("active");
}

document.addEventListener("DOMContentLoaded", function () {
  const counters = document.querySelectorAll(".info-box .value");
  counters.forEach((counter) => {
    let start = 0;
    const end = parseFloat(counter.textContent); // Giá trị đích
    const duration = 1000; // Thời gian (ms)
    const increment = (end - start) / (duration / 20); // Giá trị tăng mỗi lần

    const updateCounter = () => {
      start += increment;
      if (start >= end) {
        counter.textContent = end;
      } else {
        counter.textContent = Math.round(start * 100) / 100; // Làm tròn 2 chữ số
        requestAnimationFrame(updateCounter);
      }
    };

    updateCounter();
  });
});
document.addEventListener("DOMContentLoaded", () => {
  const currentDateElement = document.getElementById("current-date");

  // Lấy ngày tháng năm hiện tại
  const now = new Date();
  const dateString = now.toLocaleDateString("vi-VN", {
    weekday: "long",
    year: "numeric",
    month: "long",
    day: "numeric",
  });

  currentDateElement.textContent = `📅 Hôm nay: ${dateString}`;
});
