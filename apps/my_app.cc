// Copyright (c) 2020 [Your Name]. All rights reserved.

#include "my_app.h"

#include <cinder/GeomIo.h>
#include <cinder/app/App.h>
#include <mylibrary/GameBoard.h>
#include <pretzel/PretzelGui.h>

namespace myapp {

using cinder::app::KeyEvent;

MyApp::MyApp() :game_board_(2){};

void MyApp::setup() {
  gui = pretzel::PretzelGui::create("Puzzle settings");
  gui->setSize(cinder::vec2(10,10));
  gui->setPos(cinder::vec2(1000,10));

  gui->addSlider("Puzzle Size", &grid_size_, 2, 5);
  gui->addButton("Shuffle", &MyApp::ShuffleButton, this);
  gui->addButton("Update", &MyApp::UpdateButton, this);

  gui->addSaveLoad();
  gui->loadSettings();    // load the last saved settings automatically

  game_board_.ShuffleBoard();

  //ci::gl::enableAlphaBlending();
}

void MyApp::ShuffleButton() {
  game_board_.ShuffleBoard();
}

void MyApp::UpdateButton() {
  game_board_.Reset(grid_size_);
  game_board_.ShuffleBoard();
  if (won_game_) {
    won_game_ = false;
  }
}

void MyApp::update() {

  if (game_board_.CheckWin()) {
    won_game_ = true;
  }

}

void MyApp::draw() {
  cinder::gl::setMatricesWindow( getWindowSize() );

  cinder::gl::clear(cinder::Color::white());
  gui->drawAll();

  if (won_game_) {
    DrawEndScreen();
  } else {
    cinder::gl::translate(-360,-250);
    DrawGrid();
  }

}

void MyApp::DrawEndScreen() {
  cinder::gl::clear(cinder::Color::white());
  cinder::gl::translate(getWindowCenter().x, getWindowCenter().y);
  PrintNum();
}


void MyApp::PrintNum() {
  cinder::gl::color(cinder::Color(0,0,1));
  cinder::TextBox box;
  box.setSize(cinder::ivec2(100,100));
  box.setAlignment(cinder::TextBox::CENTER);
  box.setFont(cinder::Font("Arial", 30));
  box.setColor(cinder::Color(0,1,0));
  box.setBackgroundColor(cinder::ColorA(0,0,0,0));
  box.setText("You won!");

  const auto box_size = box.getSize();
  const cinder::vec2 locp = {50 - box_size.x / 2, 50 - box_size.y / 2};
  const auto surface = box.render();
  const auto texture = cinder::gl::Texture::create(surface);
  cinder::gl::draw(texture, locp);

}

void MyApp::DrawBoard() {
  cinder::gl::translate( getWindowCenter().x, getWindowCenter().y);
  cinder::gl::translate(-200,-100);

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {

      cinder::gl::color( cinder::Color( 0, 1, 0 ) );
      cinder::Area area(0,0,300,200);

      if (tile_x_.size() < game_board_.board_size_) {
        tile_x_.push_back(340 + j*300);
      }

      cinder::Rectf rect(area);
      cinder::gl::drawStrokedRect(rect);

      if (game_board_.grid_[j][i].num_ != game_board_.board_size_*game_board_.board_size_) {
        std::string str = std::to_string(game_board_.grid_[j][i].num_);
        cinder::gl::drawStringCentered (str, cinder::ivec2(150,100),
                                        cinder::ColorA(1, 0, 0, 1),
                                        cinder::Font("Arial", 30));
      }

      cinder::gl::translate(300,0);
    }
    if (tile_y_.size() < game_board_.board_size_) {
      tile_y_.push_back(250 + i*200);
    }
    cinder::gl::translate(-900,200);
  }

}

void MyApp::DrawGrid() {
  cinder::gl::translate( getWindowCenter().x, getWindowCenter().y);
  cinder::gl::translate(-200,-100);

  float x = 800 / game_board_.board_size_;
  float y = 600 / game_board_.board_size_;


  for (int i = 0; i < game_board_.board_size_; i++) {
    for (int j = 0; j < game_board_.board_size_; j++) {

      cinder::gl::color( cinder::Color( 0, 1, 0 ) );
      //cinder::Area area(0,0,200,150);

      cinder::Area area(0,0,x,y);


      if (tile_x_.size() < game_board_.board_size_) {
        tile_x_.push_back(240 + j*x);
      }


      cinder::Rectf rect(area);
      cinder::gl::drawStrokedRect(rect);


      if (game_board_.grid_[j][i].num_ != game_board_.board_size_*game_board_.board_size_) {
        std::string str = std::to_string(game_board_.grid_[j][i].num_);
        cinder::gl::drawStringCentered (str, cinder::ivec2(150,100),
                                        cinder::ColorA(1, 0, 0, 1),
                                        cinder::Font("Arial", 30));
      }



      cinder::gl::translate(x,0);
    }
    if (tile_y_.size() < game_board_.board_size_) {
      tile_y_.push_back(x + i*y);
    }
    cinder::gl::translate(-(x*game_board_.board_size_),y);
  }

}

void MyApp::keyDown(KeyEvent event) {
  if (event.getChar() == 'g'){
    gui->toggleVisible();	// gui interaction will be disabled when invisible
  } else if (event.getChar() == 'd') {
    game_board_.MoveTile(2,1,mylibrary::Direction::kDown);
  }

}
void MyApp::mouseDown(cinder::app::MouseEvent event) {
  std::cout << event.getX() << " " << event.getY() << std::endl;
  std::vector<std::vector<mylibrary::Tile>> pre_move = game_board_.grid_;


  for (int y = 0; y < tile_y_.size(); y++) {
    for (int x = 0; x < tile_x_.size(); x++) {
      if (event.getX() <= tile_x_[x] && event.getY() <= tile_y_[y]) {
        game_board_.MoveTile(x,y,mylibrary::Direction::kDown);
        if (pre_move == game_board_.grid_) {
          game_board_.MoveTile(x,y,mylibrary::Direction::kUp);
          if (pre_move == game_board_.grid_) {
            game_board_.MoveTile(x,y,mylibrary::Direction::kLeft);
            if (pre_move == game_board_.grid_) {
              game_board_.MoveTile(x,y,mylibrary::Direction::kRight);
            }
            return;
          }
          return;
        }
        return;
      }
    }
  }
}

}  // namespace myapp
