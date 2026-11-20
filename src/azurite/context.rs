use std::{cell::Cell, rc::Rc};
use windows::Win32::{
    Foundation::HWND,
    Graphics::Gdi::ValidateRect,
    UI::WindowsAndMessaging::{WM_DESTROY, WM_KEYDOWN, WM_LBUTTONDOWN, WM_PAINT},
};
use windows_window::{Window as _Window, run as _win32_run};

const WINDOW_TITLE: &str = "Azurite";

#[allow(dead_code)]
pub struct Context {
    window: Option<_Window>,
    pub size: [u32; 2],
    should_close: Rc<Cell<bool>>,
}

impl Context {
    #[cfg(target_os = "windows")]
    pub fn new(width: u32, height: u32) -> Self {
        let mut inst = Self {
            window: None,
            size: [width, height],
            should_close: Rc::new(Cell::new(false)),
        };

        let should_close_cb = Rc::clone(&inst.should_close);

        let window = _Window::new(WINDOW_TITLE)
            .on_message(move |hwnd, message, wparam, lparam| match message as u32 {
                WM_DESTROY => {
                    should_close_cb.set(true);
                    None
                }
                WM_PAINT => {
                    unsafe { _ = ValidateRect(HWND(hwnd), None) };
                    Some(0)
                }
                WM_LBUTTONDOWN => {
                    let x = (lparam & 0xffff) as i16;
                    let y = ((lparam >> 16) & 0xffff) as i16;
                    println!("WM_LBUTTONDOWN at ({x}, {y})");
                    Some(0)
                }
                WM_KEYDOWN => {
                    println!("WM_KEYDOWN, virtual key {}", wparam as u32);
                    Some(0)
                }
                _ => None,
            })
            .size(width as i32, height as i32)
            .create()
            .expect("Failed to create window");

        inst.window = Some(window);
        inst
    }

    pub fn should_close(&self) -> Rc<Cell<bool>> {
        Rc::clone(&self.should_close)
    }

    #[cfg(target_os = "windows")]
    fn poll_events(&mut self) {
        _win32_run();
    }

    pub fn update(&mut self, _delta: f32) {
        self.poll_events();
    }

    pub fn render(&mut self) {}
}

// Cross-platform multi-implementation:
// #[cfg(target_os = "PLATFORM_HERE")]
// #[cfg(target_os = "PLATFORM_HERE")]
