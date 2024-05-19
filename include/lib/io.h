/*
  including basic internal function like

  console.log
  cake.readInt();

  cake.readFileSync(filename);
  cake.writeFileSync();
  cake.readString();
  cake.readLine(0;)
*/

#pragma once
namespace cake {
class Context;
}
namespace cake::lib {
void import_console(Context *con);
void import_cake_basic(Context *con);
} // namespace cake::lib